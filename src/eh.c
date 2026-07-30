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

// example API functions for the editor
EHL_CDEF(editor_setfont) {
    const char *font_id = lua_tostring(L, -1);
    printf("[EH:CONFIG] Set editor font to %s\n", font_id);
    EHL_VOID;
}

EHL_CDEF(terminal_setfont) {
    const char *font_id = lua_tostring(L, -1);
    printf("[EH:CONFIG] Set terminal font to %s\n", font_id);
    EHL_VOID;
}

EHL_CDEF(keybinds_undo) {
    const char *keybind = lua_tostring(L, -1);
    printf("[EH:CONFIG] Set undo keybind to \"%s\"\n", keybind);
    EHL_VOID;
}

EHL_CDEF(keybinds_redo) {
    const char *keybind = lua_tostring(L, -1);
    printf("[EH:CONFIG] Set redo keybind to \"%s\"\n", keybind);
    EHL_VOID;
}

EHL_CDEF(keybinds_showterm) {
    const char *keybind = lua_tostring(L, -1);
    printf("[EH:CONFIG] Set showterm keybind to \"%s\"\n", keybind);
    EHL_VOID;
}

// https://www.lua.org/manual/5.1/manual.html#3
// https://www.codingwiththomas.com/blog/a-lua-c-api-cheat-sheet
#define EHL_INIT "./config/init.lua"

static
int32_t ehL_openapi(lua_State *L) {
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
    lua_setglobal(L, "eh");
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
