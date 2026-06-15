#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define ARENA_BASE_POSITION sizeof(arena)

typedef struct {
  uint64_t pos;
  uint64_t capacity;
} arena;

arena *createArena(uint64_t capacity) {
  arena *memarena =
      (arena *)mmap(NULL, ARENA_BASE_POSITION + capacity,
                    PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
  memarena->capacity = capacity;
  memarena->pos = ARENA_BASE_POSITION;
  return memarena;
}

void *arenaPush(arena *memarena, uint64_t size) {
  if ((memarena->pos + size) > (memarena->capacity + ARENA_BASE_POSITION)) {
    return NULL;
  }
  uint8_t *out = (uint8_t *)memarena + size;
  memarena->pos = memarena->pos + size;
  return out;
}
void freeArena(arena *memarena) {
  munmap(memarena, memarena->capacity + sizeof(arena));
}

int main(void) {
  arena *testArena = createArena(1024);
  int *test = arenaPush(testArena, sizeof(int));
  *test = 8;
  printf("test: %d", *test);
  freeArena(testArena);
  return 0;
}
