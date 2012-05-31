#include "array.h"

//#include <stdio.h>
//#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static void stackdump(lua_State *L)
{
  int i;
  int top = lua_gettop(L);
  for(i = 1; i <= top; i++)
  {
    int t = lua_type(L, i);
    printf("%3d. ", i);
    switch(t)
    {
      case LUA_TSTRING:
        printf("'%s'", lua_tostring(L,i));
        break;
      case LUA_TBOOLEAN:
        printf(lua_toboolean(L, i) ? "true" : "false");
        break;
      case LUA_TNUMBER:
        printf("%g", lua_tonumber(L,i));
        break;
      default:
        printf("%s", lua_typename(L,t));
        break;
    }
    printf("\n");
  }
  printf("---------------------------------------------\n\n");
}

static int newarray(lua_State *L) {
  int n = luaL_checkint(L, 1);
  size_t nbytes = sizeof(NumArray) + (n - 1)*sizeof(double);
  NumArray *a = (NumArray *)lua_newuserdata(L, nbytes);
   
  luaL_getmetatable(L, "LuaBook.array");
  lua_setmetatable(L, -2);

  a->size = n;
  return 1;  /* new userdatum is already on the stack */
}

static NumArray *checkarray (lua_State *L) {
  void *ud = luaL_checkudata(L, 1, "LuaBook.array");
  luaL_argcheck(L, ud != NULL, 1, "`array' expected");
  return (NumArray *)ud;
}

static double *getelem (lua_State *L) {
  NumArray *a = checkarray(L);
  int index = luaL_checkint(L, 2);
  
  luaL_argcheck(L, 1 <= index && index <= a->size, 2,
		"index out of range");
  
  /* return element address */
  return &a->values[index - 1];
}

static int setarray (lua_State *L) {
  double newvalue = luaL_checknumber(L, 3);
  *getelem(L) = newvalue;
  return 0;
}

static int getarray (lua_State *L) {
  lua_pushnumber(L, *getelem(L));
  return 1;
}

static int getsize(lua_State *L) {
  NumArray *a = checkarray(L);
  lua_pushnumber(L, a->size);
  return 1;
}

int array2string (lua_State *L) {
  NumArray *a = checkarray(L);
  lua_pushfstring(L, "array(%d)", a->size);
  return 1;
}



static const struct luaL_reg arraylib [] = {
  {"new", newarray},
  {"set", setarray},
  {"get", getarray},
  {"size", getsize},
  {"__tostring", array2string},
  {NULL, NULL}
};



int luaopen_libarray (lua_State *L) {
  luaL_newmetatable(L, "LuaBook.array");
  luaL_openlib(L, "array", arraylib, 0);

  /* now the stack has the metatable at index 1 and
     `array' at index 2 */
  lua_pushstring(L, "__index");
  lua_pushstring(L, "get");
  lua_gettable(L, -3);  /* get array.get */
  lua_settable(L, -4);  /* metatable.__index = array.get */
  

  lua_pushstring(L, "__newindex");
  lua_pushstring(L, "set");
  lua_gettable(L, -3); /* get array.set */
  lua_settable(L, -4); /* metatable.__newindex = array.set */
  
  return 0;
}
