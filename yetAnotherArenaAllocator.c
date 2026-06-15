#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  uint64_t pos;
  uint64_t capacity;
} arena;

arena *createArena(uint64_t capacity) {
  arena *memarena = (arena *)malloc(sizeof(arena) + capacity);
  memarena->capacity = capacity;
  memarena->pos = 0;
  return memarena;
}

void *arenaPush(arena *memarena, uint64_t size) {
  if (memarena->pos + size > memarena->capacity) {
    return NULL;
  }
  uint8_t *out = (uint8_t *)memarena + size;
  memarena->pos = memarena->pos + size;
  return out;
}
void freeArena(arena *memarena) { free(memarena); }

int main(void) {
  arena *testArena = createArena(1024);
  int *test = (int *)arenaPush(testArena, sizeof(int));
  *test = 10;
  printf("test: %d", *test);
  free(testArena);
  return 0;
}
