
#    ▄▄▄   ▄▄▄                   ▄▄
#    ███   ███                   ██
#    ▀███▄███▀ ▄███▄ ████▄ ▄█▀█▄ ████▄
#      ▀███▀   ██ ██ ██ ▀▀ ██▄█▀ ██ ██
#       ███    ▀███▀ ██    ▀█▄▄▄ ██ ██
#
#    Yoreh makefile

CC     ?= cc
CFLAGS ?= -march=native -mno-avx512f -NDEBUG

# ============================================================================== #
#                          DO NOT EDIT PAST THIS POINT                           #
# ============================================================================== #

AR := ar rcs

override \
CFLAGS += -std=c99 -O2

# vendor pathing
VENDOR_LUA_PREFIX  := ../vendor/lua/
ifeq ($(OS),Windows_NT)
VENDOR_LUA_PREFIX  := $(subst /,\,$(VENDOR_LUA_PREFIX))
endif

override \
CFLAGS += -I$(VENDOR_LUA_PREFIX)

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
	      lstrlib.o loadlib.o linit.o

# fix vendored paths
LUA_T  := $(addprefix $(VENDOR_LUA_PREFIX),$(LUA_T))
LUA_O  := $(addprefix $(VENDOR_LUA_PREFIX),$(LUA_O))

ALL_O  := $(EH_O) $(LUA_O)
ALL_T  := $(EH_T) $(LUA_T)
ALL    := $(EH_E) $(ALL_T) $(ALL_O)

$(EH_E): $(ALL_T)
	$(CC) $^ -o $@

$(EH_T): $(EH_O)
	$(AR) $@ $^

$(LUA_T): $(LUA_O)
	$(AR) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: eh eh-devel clean

eh: eh-devel

eh-devel: $(ALL_T)
	$(CC) $(ALL_T) -o $(EH_E)

clean:
ifeq ($(OS),Windows_NT)
	del $(ALL) 2>nul
else
	rm $(ALL)
endif

.DEFAULT_GOAL := eh
