#ifndef LUA_H
#define LUA_H

//#include <lua5.2/lua.hpp>
#include <lua.hpp>
#include <iostream>
using namespace std;


static void report_errors(lua_State *L, const int status)
{
  if ( status!=0 ) {
    std::cerr << "-- " << lua_tostring(L, -1) << std::endl;
    lua_pop(L, 1); // remove error message
  }
}

class LuaScript
{
public:
    LuaScript();

    ~LuaScript() {
      lua_close(L);
    }
    void Execute(string path);
    // implicitly act as a lua_State pointer
    inline operator lua_State*() {
      return L;
    }

    //nicht kopierbar!
    LuaScript(LuaScript const&) = delete;
    LuaScript& operator=(LuaScript const&) = delete;
    LuaScript(LuaScript&&) = default;
private:
      lua_State *L;
};

#endif // LUA_H
