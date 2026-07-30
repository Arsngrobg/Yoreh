
#    ▄▄▄   ▄▄▄                   ▄▄
#    ███   ███                   ██
#    ▀███▄███▀ ▄███▄ ████▄ ▄█▀█▄ ████▄
#      ▀███▀   ██ ██ ██ ▀▀ ██▄█▀ ██ ██
#       ███    ▀███▀ ██    ▀█▄▄▄ ██ ██
#
#    Yoreh makefile

BUILD_COCOA   ?= 0
BUILD_WIN32   ?= 0
BUILD_WAYLAND ?= 0
BUILD_X11     ?= 0

CC     ?= cc
CFLAGS ?= -march=native -DNDEBUG

# ============================================================================== #
#                          DO NOT EDIT PAST THIS POINT                           #
# ============================================================================== #

AR := ar rcs

override \
CFLAGS += -std=c99 -O2

# vendor pathing
VENDOR_LUA_PREFIX  := ../vendor/lua/
VENDOR_GLFW_SRC_PREFIX := ../vendor/glfw/src/
VENDOR_GLFW_INC_PREFIX := ../vendor/glfw/include/
ifeq ($(OS),Windows_NT)
VENDOR_LUA_PREFIX      := $(subst /,\,$(VENDOR_LUA_PREFIX))
VENDOR_GLFW_SRC_PREFIX := $(subst /,\,$(VENDOR_GLFW_SRC_PREFIX))
VENDOR_GLFW_INC_PREFIX := $(subst /,\,$(VENDOR_GLFW_INC_PREFIX))
endif

override \
CFLAGS += -I$(VENDOR_LUA_PREFIX) -I$(VENDOR_GLFW_INC_PREFIX)

# targets and object defs
EH_E := eh
ifeq ($(OS),Windows_NT)
    EH_E := $(addsuffix .exe,$(EH_E))
endif

EH_T   := libeh.a
EH_O   := eh.o

LUA_T  := liblua.a
LUA_O  := lapi.o     lcode.o    ldebug.o   ldo.o      ldump.o    lfunc.o  lgc.o     \
          llex.o     lmem.o     lobject.o  lopcodes.o lparser.o  lstate.o lstring.o \
		  ltable.o   ltm.o      lundump.o  lvm.o      lzio.o                        \
                                                                                    \
          lauxlib.o  lbaselib.o ldblib.o   liolib.o   lmathlib.o loslib.o ltablib.o \
	      lstrlib.o  loadlib.o  linit.o

GLFW_T := libglfw.a
GLFW_O := context.o init.o input.o monitor.o platform.o vulkan.o window.o                         \
          egl_context.o osmesa_context.o null_init.o null_monitor.o null_window.o null_joystick.o

# glfw OS-dependent modules
ifeq ($(OS),Windows_NT)
    LIBS   += -lgdi32 -luser32 -lkernel32 -lopengl32 -lshlwapi
    GLFW_O += win32_module.o win32_time.o win32_thread.o
else
    UNAME := $(shell uname -s)
    ifeq ($(UNAME),Linux)
        LIBS   += -lGL -lm -pthread -ldl
        GLFW_O += posix_module.o posix_time.o posix_thread.o
    endif
    ifeq ($(UNAME),Darwin)
        LIBS   += -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL
        GLFW_O += posix_module.o cocoa_time.o posix_thread.o
    endif
endif

# force build system to include every possible object file when 'clean' recipe is used
ifneq ($(filter clean,$(MAKECMDGOALS)),)
    BUILD_COCOA   := 1
    BUILD_WIN32   := 1
    BUILD_WAYLAND := 1
    BUILD_X11     := 1
endif

ifeq ($(BUILD_COCOA),1)
    override \
    CFLAGS += -D_GLFW_COCOA
    GLFW_O += cocoa_init.o cocoa_monitor.o cocoa_window.o cocoa_joystick.o \
	          nsgl_context.o
endif
ifeq ($(BUILD_WIN32),1)
    override \
    CFLAGS += -D_GLFW_WIN32
    GLFW_O += win32_init.o win32_monitor.o win32_window.o win32_joystick.o \
	          wgl_context.o
endif
ifeq ($(BUILD_X11),1)
    override \
    CFLAGS += -D_GLFW_X11
    GLFW_O += x11_init.o x11_monitor.o x11_window.o \
	          glx_context.o
endif
ifeq ($(BUILD_WAYLAND),1)
    override \
    CFLAGS += -D_GLFW_WAYLAND
    GLFW_O += wl_init.o wl_monitor.o wl_window.o
endif

ifeq ($(or $(BUILD_X11),$(BUILD_WAYLAND)),1)
    GLFW_O += xkb_unicode.o
    UNAME := $(shell uname -s)
	ifeq ($(UNAME),Linux)
        GLFW_O += linux_joystick.o
	else
        GLFW_O += posix_poll.o
	endif
endif

# fix vendored paths
LUA_T  := $(addprefix $(VENDOR_LUA_PREFIX),     $(LUA_T))
GLFW_T := $(addprefix $(VENDOR_GLFW_SRC_PREFIX),$(GLFW_T))
LUA_O  := $(addprefix $(VENDOR_LUA_PREFIX),     $(LUA_O))
GLFW_O := $(addprefix $(VENDOR_GLFW_SRC_PREFIX),$(GLFW_O))

ALL_O  := $(EH_O) $(LUA_O) $(GLFW_O)
ALL_T  := $(EH_T) $(LUA_T) $(GLFW_T)
ALL    := $(EH_E) $(ALL_T) $(ALL_O)

$(EH_T): $(EH_O)
	$(AR) $@ $^

$(GLFW_T): $(GLFW_O)
	$(AR) $@ $^

$(LUA_T): $(LUA_O)
	$(AR) $@ $^

%.o: %.m
	$(CC) $(CFLAGS) -x c -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: eh eh-devel clean

eh: eh-devel

eh-devel: $(ALL_T)
	$(CC) $(ALL_T) -o $(EH_E) $(LIBS)

clean:
ifeq ($(OS),Windows_NT)
	del $(ALL) 2>nul
else
	rm $(ALL)
endif

.DEFAULT_GOAL := eh
