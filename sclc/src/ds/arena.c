/*
 * arena: simple chained arena implementation, initial implementation inspired
 *        from "i hate malloc/free with a passion" by MagicalBat on YouTube
 *        (https://youtu.be/jgiMagdjA1s)
 *
 * Scull Project Copyright (C) 2026, Samyak Bambole <bambole@duck.com>
 * Licensed under the GNU/GPL Version 3
 */

#include "ds/arena.h"
#include "common.h"
#include "utils.h"

#include <stdalign.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define ARENA_DEFAULT_BLOCK_SIZE (1 << 20)

void arena_init(mem_arena *arena) {
  arena->default_block_size = (1 << 20);
  arena->first = scu_checked_malloc(sizeof(mem_arena_block));
  arena->first->buffer = scu_checked_malloc(arena->default_block_size);
  arena->first->capacity = arena->default_block_size;
  arena->first->pos = 0;
  arena->first->next = NULL;
  arena->current = arena->first;
}

void *arena_push(mem_arena *arena, u64 size) {
  u64 pos_aligned = (arena->current->pos + (alignof(max_align_t)) - 1) &
                    ~((alignof(max_align_t)) - 1);

  u64 new_pos = pos_aligned + size;

  if (new_pos > arena->current->capacity) {
    u64 new_block_size = arena->default_block_size;
    if (new_block_size < size)
      new_block_size = size;

    mem_arena_block *new_block = scu_checked_malloc(sizeof(mem_arena_block));
    new_block->buffer = scu_checked_malloc(new_block_size);
    new_block->capacity = new_block_size;
    new_block->pos = 0;
    new_block->next = NULL;

    arena->current->next = new_block;
    arena->current = new_block;

    pos_aligned = 0;
    new_pos = size;
  }

  void *out = arena->current->buffer + pos_aligned;
  arena->current->pos = new_pos;
  memset(out, 0, size);
  return out;
}

void arena_pop(mem_arena *arena, u64 size) {
  while (size > 0 && arena->current) {
    if (size <= arena->current->pos) {
      arena->current->pos -= size;
      return;
    }

    size -= arena->current->pos;
    arena->current->pos = 0;

    if (arena->current == arena->first)
      return;

    mem_arena_block *prev = arena->first;
    while (prev->next != arena->current)
      prev = prev->next;
    arena->current = prev;
  }
}

void arena_clear(mem_arena *arena) {
  mem_arena_block *block = arena->first;
  while (block) {
    block->pos = 0;
    block = block->next;
  }
  arena->current = arena->first;
}

void arena_free(mem_arena *arena) {
  mem_arena_block *block = arena->first;
  while (block) {
    mem_arena_block *next = block->next;
    free(block->buffer);
    free(block);
    block = next;
  }
  arena->first = NULL;
  arena->current = NULL;
}
