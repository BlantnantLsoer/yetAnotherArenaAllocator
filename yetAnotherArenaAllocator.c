#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define ARENA_BASE_POSITION sizeof(arena)
#define GiB(x) (x) << 30
#define MiB(x) (x) << 20
#define KiB(x) (x) << 10
#define ARENA_PUSH(arena, type)                                                \
  (type *)arenaPush((arena), sizeof(type), alignof(type))

#define ALIGN(offset, align) (((offset) + ((align) - 1)) & ~((align) - 1))

typedef struct {
  uint64_t offset;
  uint64_t capacity;
} arena;

arena *createArena(uint64_t capacity) {
  arena *ptr =
      (arena *)mmap(NULL, ARENA_BASE_POSITION + capacity,
                    PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
  ptr->capacity = capacity;
  ptr->offset = 0;
  ptr++;
  return ptr;
}

void *arenaPush(arena *memarena, uint64_t size, size_t align) {
  arena *header = memarena - 1;
  if ((header->offset + size) > (header->capacity)) {
    return NULL;
  }
  uint64_t aligned = ALIGN(header->offset, align);
  uint8_t *out = (uint8_t *)memarena + aligned;
  header->offset = aligned + size;
  return out;
}

void freeArena(arena *memarena) {
  arena *header = memarena - 1;
  munmap(header, header->capacity + ARENA_BASE_POSITION);
}

void arenaReset(arena *memarena) { memarena->offset = 0; }

int main(void) {
  arena *testArena = createArena(KiB(4));
  char *test = ARENA_PUSH(testArena, char);
  int *test2 = ARENA_PUSH(testArena, int);
  char *test3 = ARENA_PUSH(testArena, char);
  char *test4 = ARENA_PUSH(testArena, char);
  int *test5 = ARENA_PUSH(testArena, int);
  *test = 'a';
  *test2 = 30;
  *test3 = 'b';
  *test4 = 'd';
  *test5 = 126;
  printf("test: %c\n", *test);
  printf("test2: %d\n", *test2);
  printf("test3: %c\n", *test3);
  printf("test4: %c\n", *test4);
  printf("test5: %d\n", *test5);
  freeArena(testArena);
  return 0;
}
