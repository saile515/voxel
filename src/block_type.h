#pragma once

#include <stdbool.h>

typedef struct BlockTypeStruct {
  bool is_solid : 1;
  unsigned int id : 15;
} BlockTypeStruct;

typedef const BlockTypeStruct *BlockType;

static const BlockTypeStruct AIR = {false, 0};
static const BlockTypeStruct GRASS = {true, 1};
