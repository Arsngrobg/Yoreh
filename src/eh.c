#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define EH_KEYBIND_MAXKEYS (15)

// keybind data
typedef struct {
    int32_t keys[EH_KEYBIND_MAXKEYS+1]; // keycodes
    int32_t mods;                       // any modifier keys (bitmask)
    int32_t regid;                      // ID of the lua callback function
} eh_Keybind;

// app data
typedef struct {
    GLFWwindow *win;      // window
    lua_State  *L;        // lua vm
    eh_Keybind *keybinds; // keybind data
} eh_State;

// CALLBACKS
static
void eh_GLFWerrorfun(int32_t error_code, const char *description) {
    fprintf(stderr, "[EH] (FAILURE): %s (exit code: 0x%x)\n", description, error_code);
}

static
void eh_GLFWkeycallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

}

static
void eh_GLFWwindowsizecallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// LUA API
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
#define EH_INIT_LUA "./config/init.lua"

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

    return EXIT_SUCCESS;
}

// STATE MANAGEMENT
static
int32_t eh_start(eh_State *eh) {
    // glfw
    glfwSetErrorCallback(eh_GLFWerrorfun);
    if (glfwInit() == GLFW_FALSE) {
        return EXIT_FAILURE;
    }

    const GLFWvidmode *vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_COMPAT_PROFILE);
    //glfwWindowHint(GLFW_DECORATED,             GLFW_FALSE);
    glfwWindowHint(GLFW_POSITION_X,            vidmode->width/4);
    glfwWindowHint(GLFW_POSITION_Y,            vidmode->height/4);

    eh->win = glfwCreateWindow(vidmode->width/2, vidmode->height/2, "Yoreh", NULL, NULL);
    if (eh->win == NULL) {
        return EXIT_FAILURE;
    }

    glfwSetKeyCallback       (eh->win, eh_GLFWkeycallback);
    glfwSetWindowSizeCallback(eh->win, eh_GLFWwindowsizecallback);

    glfwMakeContextCurrent(eh->win);
    gladLoadGL();
    glfwSwapInterval(1);

    // lua
    eh->L = luaL_newstate();
    if (eh->L == NULL) {
        fprintf(stderr, "[EH] (FAILURE): Unable to initialize Lua backend\n");
        return EXIT_FAILURE;
    }

    luaopen_base(eh->L);
    luaopen_math(eh->L);
    ehL_openapi (eh->L);

    if (luaL_dofile(eh->L, EH_INIT_LUA) == 0) {
        printf("[EH] Executed "EH_INIT_LUA"\n");
    } else {
        fprintf(stderr, "[EH] (FAILURE) %s\n", lua_tostring(eh->L, -1));
        lua_pop(eh->L, 1);
    }

    return EXIT_SUCCESS;
}

static
int32_t eh_run(eh_State *eh) {
    while (!glfwWindowShouldClose(eh->win)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.12, 0.12, 0.12, 1.00);

        glColor3f(0.05, 0.05, 0.05);
        glBegin(GL_QUADS);
            glVertex2f(-0.99,  0.98);
            glVertex2f(-0.99, -0.98);
            glVertex2f( 0.99, -0.98);
            glVertex2f( 0.99,  0.98);
        glEnd();

        glfwSwapBuffers(eh->win);
        glfwWaitEvents(); // glfwPollEvents();
    }

    return EXIT_SUCCESS;
}

static
int32_t eh_destroy(eh_State *eh) {
    glfwDestroyWindow(eh->win);
    glfwTerminate();
    return EXIT_SUCCESS;
}

int32_t main(void) {
    eh_State eh = {0};
    return !eh_start(&eh) && !eh_run(&eh) && !eh_destroy(&eh);
}
