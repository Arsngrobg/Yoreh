#include <stdint.h>
#include <stdio.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define EHLUA_RETURNS(n)  (n)
#define EHLUA_VOID_RETURN EHLUA_RETURNS(0)

// example API functions for the editor
int32_t eh_editor_setfont(lua_State *L) {
    const char *font_id = lua_tostring(L, -1);
    printf("[EH (PHONY)] Set editor font to %s\n", font_id);
    return EHLUA_VOID_RETURN;
}

#define EHLUA_INIT "./config/init.lua"

// https://www.lua.org/manual/5.1/manual.html#3
// https://www.codingwiththomas.com/blog/a-lua-c-api-cheat-sheet
int32_t main(void) {
    printf("Yoreh-ditor!\n");
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushcclosure(L, eh_editor_setfont, 0);
    lua_setglobal(L, "setfont");

    if (luaL_dofile(L, EHLUA_INIT) == 0) {
        printf("[C] Executed "EHLUA_INIT"\n");
    } else {
        printf("[C] Error reading script\n");
        luaL_error(L, "Error: %s", lua_tostring(L, -1));
    }

    lua_close(L);
    return 0;
}
