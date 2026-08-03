#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
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

static
void eh_GLFWkeycallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static
void eh_GLFWwindowsizecallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int32_t main(void) {
    printf("Yoreh-ditor!\n");
    lua_State *L = ehL_init();

    lua_close(L);

    glfwSetErrorCallback(eh_GLFWerrorfun);
    if (glfwInit() == GLFW_FALSE) {
        return EXIT_FAILURE;
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwWindowHint(GLFW_POSITION_X, (mode->width-640)/2);
    glfwWindowHint(GLFW_POSITION_Y, (mode->height-480)/2);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Yoreh-ditor", NULL, NULL);
    glfwSetKeyCallback(window, eh_GLFWkeycallback);
    glfwSetWindowSizeCallback(window, eh_GLFWwindowsizecallback);
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1, 1, 1, 1);

        glColor3f(0.1, 0.2, 0.3);
        glBegin(GL_QUADS);
            glVertex2f(-1.0f, 1.0f);
            glVertex2f(-1.0f, 0.0f);
            glVertex2f(1.0f, 0.0f);
            glVertex2f(1.0f, 1.0f);
        glEnd();

        glfwWaitEvents(); // glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
