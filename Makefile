# librt is not on Mac but only on Linux
# CIC machine is Linux; exclude librt on Mac
UNAME_S := $(shell uname -s)


EXEC = cada0030
BIN_DIR = bin
OPT_DIR = $(BIN_DIR)/opt
DBG_DIR = $(BIN_DIR)/dbg
OPT_EXEC = $(OPT_DIR)/$(EXEC)
DBG_EXEC = $(DBG_DIR)/$(EXEC)
TRN_EXEC = $(OPT_DIR)/exec_trn
DBGFLAG = _DEBUG_
MAIN_CPP = src/main.cpp
MAIN_O = obj/main.o
HFILE := $(wildcard src/*.h)
CPPFILE := $(wildcard src/*.cpp) # a list of files end in .cpp
CPPFILE := $(filter-out src/test_adjacencylist.cpp,$(CPPFILE))
OFILE := $(addprefix obj/,$(addsuffix .o,$(basename $(notdir $(CPPFILE))))) # replace src/*.cpp w/ obj/*.o in CPPFILE
OFILE := $(filter-out obj/main.o,$(OFILE)) # remove main.o
CC = g++
CFLAGS = --std=c++11 -Wall -Wextra -g
LIB_DIR = abc

DIR = src/
SOURCE = $(DIR)util.o $(DIR)cirMgr.o


ifeq ($(UNAME_S),Linux)
	LIB := abc rt m dl pthread
else
	LIB := abc m dl pthread
endif
LIB := $(addprefix -l,$(LIB))
AFILE = $(LIB_DIR)/libabc.a

.PHONY = all dbg clean al lib translate

all translate: CFLAGS += -O3
dbg: CFLAGS += -g3 -O0 -ggdb $(addprefix -D, $(DBGFLAG))

all: $(OPT_EXEC)
	@# Link the exec from under bin to here
	@ln -sf $< .
	@mv $(EXEC) xec
	@# the name of the executable must be so

dbg: $(DBG_EXEC)
	@ln -sf $< .

translate: $(TRN_EXEC)
	@ln -sf $< .

$(OPT_EXEC): $(MAIN_CPP) $(AFILE) $(SOURCE) | $(OPT_DIR) obj
	$(info `` Compiling main...)
	@$(CC) $(CFLAGS) -c $(MAIN_CPP) -o $(MAIN_O)
	@echo "> compiling: src/main.cpp"
	@$(CC) -L$(LIB_DIR) $(LIB) $(CFLAGS) $(MAIN_O) $(AFILE) $(SOURCE) -o $(OPT_EXEC) -ldl -lpthread
	@echo "> compiling: abc"
	@echo "> building xec..."
	@echo "To execute xec, enter:"
	@echo "./xec <golden.v> <revised.v> <output>"


$(DBG_EXEC): $(MAIN_CPP) $(AFILE) $(SOURCE) | $(DBG_DIR) obj
	$(info `` Compiling main with debugging information...)
	@$(CC) $(CFLAGS) -c $(MAIN_CPP) -o $(MAIN_O)
	@echo "> compiling: src/main.cpp"
	@$(CC) -L$(LIB_DIR) $(LIB) $(CFLAGS) $(MAIN_O) $(AFILE) $(SOURCE) -o $(DBG_EXEC) -ldl -lpthread
	@echo "> compiling: abc"
	@echo "> building xec..."
	@echo "To execute xec, enter:"
	@echo "./xec <golden.v> <revised.v> <output>"

lib $(AFILE):
	@# create lib in abc
	@# use 'touch abc/src/base/abc/abc.h' to make libabc.a outdated
	$(info `` Compiling ABC...)
	@make -C abc libabc.a ABC_USE_NO_READLINE=1 --no-print-directory

$(TRN_EXEC): src/translate.cpp | abc/abc $(OPT_DIR)
	@# compile src/translate.cpp
	$(info `` Compiling translater program...)
	$(CC) -o $(TRN_EXEC) src/translate.cpp

abc/abc:
	@make -C abc ABC_USE_NO_READLINE=1 --no-print-directory


$(DIR)%.o : $(DIR)%.cpp  $(DIR)%.h
	@echo "> compiling: $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

### Create folders
$(OPT_DIR): | $(BIN_DIR)
	$(info `` Making directory $@...)
	@mkdir $(OPT_DIR)

$(DBG_DIR): | $(BIN_DIR)
	$(info `` Making directory $@...)
	@mkdir $(DBG_DIR)

$(BIN_DIR):
	$(info `` Making directory $@...)
	@mkdir $(BIN_DIR)

obj:
	$(info `` Making directory $@...)
	@mkdir obj

clean:
	@# removes all *.o files and binary files under bin/
	$(info `` Cleaning all intermediate files under current directory...)
	@rm -rf obj bin exec_trn $(EXEC)
	rm  $(DIR)*.o xec

clean_all:
	$(info `` Cleaning all intermediate files...)
	@rm -rf obj bin exec_trn $(EXEC)
	rm  $(DIR)*.o xec
	@make -C abc clean --no-print-directory
