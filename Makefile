SRCPATHS = \
	src/*.c \

SRC = $(wildcard $(SRCPATHS))
OBJECTS = $(SRC:src/%.c=build/%.o)

EXECUTABLE_NAME = lam2
ECHO = echo

ifeq ($(OS),Windows_NT)
	EXECUTABLE_NAME = lam2.exe
else
	ECHO = /usr/bin/echo
	# JANK FIX FOR SANDWICH'S DUMB ECHO ON HIS LINUX MACHINE
endif

CC = gcc

INCLUDEPATHS = -Isrc/
DEBUGFLAGS = -pg -g
ASANFLAGS = -fsanitize=undefined -fsanitize=address
CFLAGS = -MD -Wall -Wno-format -Wincompatible-pointer-types -Wno-discarded-qualifiers -Wno-deprecated-declarations -Wreturn-type
OPT = -O3 -flto -march=native

FILE_NUM = 0

build/%.o: src/%.c
	$(eval FILE_NUM=$(shell echo $$(($(FILE_NUM)+1))))
	$(shell $(ECHO) 1>&2 -e "\e[0m[\e[32m$(FILE_NUM)/$(words $(SRC))\e[0m]\t Compiling \e[1m$<\e[0m")
	@$(CC) -c -o $@ $< $(INCLUDEPATHS) $(CFLAGS) $(OPT)

build: $(OBJECTS)
	@$(CC) $(OBJECTS) -o $(EXECUTABLE_NAME) $(CFLAGS)

debug: CFLAGS += $(DEBUGFLAGS)
debug: OPT = -O0
debug: build

clean:
	@rm -rf build/
	@mkdir build/
	@mkdir -p $(dir $(OBJECTS))

cleanbuild: clean build

-include $(OBJECTS:.o=.d)