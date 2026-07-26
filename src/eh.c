#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

// some ease-of-use macros
#define EHL_RETURN(n) return (n)
#define EHL_VOID      EHL_RETURN(0)

#define EHL_CFUN(id) ehL_##id
#define EHL_CDEF(id) static int32_t EHL_CFUN(id)(lua_State *L)

// example API functions for the editor
EHL_CDEF(editor_setfont) {
    const char *font_id = lua_tostring(L, -1);
    printf("[EH (PHONY)] Set editor font to %s\n", font_id);
    EHL_VOID;
}

EHL_CDEF(terminal_setfont) {
    const char *font_id = lua_tostring(L, -1);
    printf("[EH (PHONY)] Set terminal font to %s\n", font_id);
    EHL_VOID;
}

EHL_CDEF(keybinds_undo) {
    const char *keybind = lua_tostring(L, -1);
    printf("[EH (PHONY)] Set undo keybind to \"%s\"\n", keybind);
    EHL_VOID;
}

EHL_CDEF(keybinds_redo) {
    const char *keybind = lua_tostring(L, -1);
    printf("[EH (PHONY)] Set redo keybind to \"%s\"\n", keybind);
    EHL_VOID;
}

EHL_CDEF(keybinds_showterm) {
    const char *keybind = lua_tostring(L, -1);
    printf("[EH (PHONY)] Set showterm keybind to \"%s\"\n", keybind);
    EHL_VOID;
}

static int32_t ehL_initapi(lua_State *L) {
    assert(L != NULL);

    // define table that acts as the namespace for the yoreh API functions
    lua_newtable(L);
        // editor functions
        lua_pushstring(L, "editor");
        lua_newtable(L);
            lua_pushstring(L, "setfont");
            lua_pushcfunction(L, EHL_CFUN(editor_setfont));
            lua_settable(L, -3);
        lua_settable(L, -3);
        // terminal functions
        lua_pushstring(L, "terminal");
        lua_newtable(L);
            lua_pushstring(L, "setfont");
            lua_pushcfunction(L, EHL_CFUN(terminal_setfont));
            lua_settable(L, -3);
        lua_settable(L, -3);
        // keybinds functions
        lua_pushstring(L, "keybinds");
        lua_newtable(L);
            lua_pushstring(L, "showterm");
            lua_pushcfunction(L, EHL_CFUN(keybinds_showterm));
            lua_settable(L, -3);
            lua_pushstring(L, "undo");
            lua_pushcfunction(L, EHL_CFUN(keybinds_undo));
            lua_settable(L, -3);
            lua_pushstring(L, "redo");
            lua_pushcfunction(L, EHL_CFUN(keybinds_redo));
            lua_settable(L, -3);
        lua_settable(L, -3);
    lua_setglobal(L, "yoreh");
}

// https://www.lua.org/manual/5.1/manual.html#3
// https://www.codingwiththomas.com/blog/a-lua-c-api-cheat-sheet
#define EHLUA_INIT "./config/init.lua"

int32_t main(void) {
    printf("Yoreh-ditor!\n");
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    ehL_initapi(L);

    if (luaL_dofile(L, EHLUA_INIT) == 0) {
        printf("[C] Executed "EHLUA_INIT"\n");
    } else {
        printf("[C] Error reading script\n");
        luaL_error(L, "Error: %s", lua_tostring(L, -1));
    }

    lua_close(L);
    return 0;
}
