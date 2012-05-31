#ifndef array_h
#define array_h

#include "lua.h"

typedef struct NumArray {
  int size;
  double values[1];  /* variable part */
} NumArray;

int luaopen_array(lua_State *L);

#endif
