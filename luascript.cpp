#include "luascript.h"

LuaScript::LuaScript():L(luaL_newstate())
{
    luaL_openlibs(L);
}

void LuaScript::Execute(string path)
{
    // load Lua libraries
    /*
    static const luaL_Reg lualibs[] =
    {
        { "base", luaopen_base },
        { NULL, NULL}
    };

    const luaL_Reg *lib = lualibs;
    for(; lib->func != NULL; lib++)
    {
        lib->func(L);
        lua_settop(L, 0);
    }
    */
    int e=luaL_dofile(L, path.c_str());

    lua_getglobal( L, "test" );
    lua_pcall(L, 0,0,0);
    report_errors(L, e);

}
