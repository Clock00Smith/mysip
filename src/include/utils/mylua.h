#pragma once

#include <lua5.3/lua.hpp>
#include <iostream>
extern "C" int luaopen_mylua(lua_State* L);
static int log(lua_State *L){
    std::cout << "log!" << std::endl;
}
static const luaL_Reg m_lib[] {
    {"c_log", log},
    {NULL, NULL}
};

int luaopen_mylua(lua_State* L){
    luaL_newlib(L, m_lib);
    return 1;
}