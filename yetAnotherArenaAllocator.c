#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define ARENA_BASE_POSITION sizeof(arena)
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

void *arenaPush(arena *memarena, uint64_t size) {
  arena *header = memarena - 1;
  if ((header->offset + size) > (header->capacity)) {
    return NULL;
  }
  uint8_t *out = (uint8_t *)memarena + header->offset;
  header->offset += size;
  return out;
}

void freeArena(arena *memarena) {
  arena *header = memarena - 1;
  munmap(header, header->capacity + ARENA_BASE_POSITION);
}

void arenaReset(arena *memarena) { memarena->offset = 0; }
int main(void) {
  arena *testArena = createArena(1024 * 1024 * 1024);
  int *test = arenaPush(testArena, sizeof(int));
  int *test2 = arenaPush(testArena, sizeof(int));
  int *test3 = arenaPush(testArena, sizeof(int));
  int *test4 = arenaPush(testArena, sizeof(int));
  *test = 10;
  *test2 = 20;
  *test3 = 30;
  *test4 = 40;
  printf("test: %d\n", *test);
  printf("test2: %d\n", *test2);
  printf("test3: %d\n", *test3);
  printf("test4: %d\n", *test4);
  freeArena(testArena);
  return 0;
}
