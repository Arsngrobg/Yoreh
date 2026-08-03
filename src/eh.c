#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "GLFW/glfw3.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

// some ease-of-use macros
#define EHL_RETURN(n) return (n)
#define EHL_VOID      EHL_RETURN(0)

#define EHL_CFUN(id) ehL_##id
#define EHL_CDEF(id) static int32_t EHL_CFUN(id)(lua_State *L)

// API functions
EHL_CDEF(commands_add) {
    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    EHL_VOID;
}

EHL_CDEF(commands_del) {
    luaL_checktype(L, 1, LUA_TSTRING);
    EHL_VOID;
}

EHL_CDEF(keymap_set) {
    luaL_checktype(L, 1, LUA_TSTRING);
    //luaL_checktype(L, 2, LUA_TFUNCTION); // TODO: allow commands by name too
    EHL_VOID;
}

EHL_CDEF(keymap_del) {
    luaL_checktype(L, 1, LUA_TSTRING);
    EHL_VOID;
}

EHL_CDEF(console_toggle) {
    EHL_VOID;
}

// https://www.lua.org/manual/5.1/manual.html#3
// https://www.codingwiththomas.com/blog/a-lua-c-api-cheat-sheet
#define EHL_INIT "./config/init.lua"

static
int32_t ehL_openapi(lua_State *L) {
    // the table that contains the eh API namespace
    lua_newtable(L);
        // commands namespace
        lua_pushstring(L, "commands");
        lua_newtable(L);
            lua_pushstring(L, "add");
            lua_pushcfunction(L, EHL_CFUN(commands_add));
            lua_settable(L, -3);
            lua_pushstring(L, "del");
            lua_pushcfunction(L, EHL_CFUN(commands_del));
            lua_settable(L, -3);
        lua_settable(L, -3);
        // keymap namespace
        lua_pushstring(L, "keymap");
        lua_newtable(L);
            lua_pushstring(L, "set");
            lua_pushcfunction(L, EHL_CFUN(keymap_set));
            lua_settable(L, -3);
            lua_pushstring(L, "del");
            lua_pushcfunction(L, EHL_CFUN(keymap_del));
            lua_settable(L, -3);
        lua_settable(L, -3);
        // console namespace
        lua_pushstring(L, "console");
        lua_newtable(L);
            lua_pushstring(L, "enabled");
            lua_pushboolean(L, true);
            lua_settable(L, -3);
            lua_pushstring(L, "font");
            lua_pushnil(L);
            lua_settable(L, -3);
            lua_pushstring(L, "color");
            lua_pushinteger(L, 0xFFFFFF);
            lua_settable(L, -3);
            lua_pushstring(L, "shader");
            lua_pushnil(L);
            lua_settable(L, -3);
            lua_pushstring(L, "toggle");
            lua_pushcfunction(L, EHL_CFUN(console_toggle));
            lua_settable(L, -3);
        lua_settable(L, -3);
        // editor namespace
        lua_pushstring(L, "editor");
        lua_newtable(L);
            lua_pushstring(L, "font");
            lua_pushnil(L);
            lua_settable(L, -3);
            lua_pushstring(L, "cursor");
            lua_newtable(L);
                lua_pushstring(L, "x");
                lua_pushinteger(L, 0);
                lua_settable(L, -3);
                lua_pushstring(L, "y");
                lua_pushinteger(L, 0);
                lua_settable(L, -3);
            lua_settable(L, -3);
        lua_settable(L, -3);
    lua_setglobal(L, "eh");

    return 0;
}

static
lua_State *ehL_init(void) {
    lua_State *L = luaL_newstate();
    if (L == NULL) {
        fprintf(stderr, "[EH] Unable to initialize Lua context");
        return NULL;
    }

    luaopen_base(L);
    luaopen_math(L);
    ehL_openapi(L);

    if (luaL_dofile(L, EHL_INIT) == 0) {
        printf("[EH] Executed "EHL_INIT"\n");
    } else {
        luaL_error(L, "Error: %s", lua_tostring(L, -1));
    }

    return L;
}

static
void eh_GLFWerrorfun(int32_t error_code, const char *description) {
    printf("Eh-rror: %s (exit code: 0x%x)\n", description, error_code);
}

int32_t main(void) {
    printf("Yoreh-ditor!\n");
    lua_State *L = ehL_init();

    lua_close(L);

    glfwSetErrorCallback(&eh_GLFWerrorfun);
    if (glfwInit() == GLFW_FALSE) {
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_POSITION_X, (1920-640)/2);
    glfwWindowHint(GLFW_POSITION_Y, (1080-480)/2);
    GLFWwindow* window = glfwCreateWindow(640, 480, "Yoreh-ditor", NULL, NULL);
    while (!glfwWindowShouldClose(window)) {
        glfwWaitEvents(); // glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
