MAKEFILE_PATH:=$(abspath $(lastword $(MAKEFILE_LIST)))
ROOT_DIR:=$(realpath $(dir $(MAKEFILE_PATH)))
ifeq ($(OS),Windows_NT)
	platform=windows
else
	architecture=$(shell uname -m)

	ifeq ($(shell uname -s),Linux)
		platform=linux
	endif
	ifeq ($(shell uname -s),Darwin)
		platform=macos
	endif
endif
ifeq ($(platform),emscripten)
CC=emcc
CXX=em++
AR=emar
else ifeq ($(platform),ios)
else ifeq ($(platform),tvos)
endif
CXXFLAGS=-c -std=c++11 -Wall -O2
SOURCES=$(ROOT_DIR)/../sr/BMP.cpp \
	$(ROOT_DIR)/../sr/Color.cpp \
	$(ROOT_DIR)/../sr/Matrix4.cpp \
	$(ROOT_DIR)/../sr/Renderer.cpp

ifeq ($(platform),windows)
CXXFLAGS+=-DNOMINMAX -DWINVER=0x0601 -D_WIN32_WINNT=0x0601
else ifeq ($(platform),ios)
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
else ifeq ($(platform),tvos)
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
endif
BASE_NAMES=$(basename $(SOURCES))
OBJECTS=$(BASE_NAMES:=.o)
LIBRARY=libsr.a
DEPENDENCIES=$(OBJECTS:.o=.d)

.PHONY: all
ifeq ($(debug),1)
all: CXXFLAGS+=-DDEBUG -g
all: CFLAGS+=-DDEBUG -g
endif
all: $(ROOT_DIR)/$(LIBRARY)

$(ROOT_DIR)/$(LIBRARY): $(OBJECTS)
	$(AR) rs $@ $^

-include $(DEPENDENCIES)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP $< -o $@

.PHONY: clean
clean:
ifeq ($(platform),windows)
	-del /f /q "$(ROOT_DIR)\$(LIBRARY)"
	-del /s "$(ROOT_DIR)\..\*.o"
	-del /s "$(ROOT_DIR)\..\*.d"
else
	$(RM) $(ROOT_DIR)/$(LIBRARY)
	find $(ROOT_DIR)/.. -name '*.o' -type f -delete
	find $(ROOT_DIR)/.. -name '*.d' -type f -delete
endif