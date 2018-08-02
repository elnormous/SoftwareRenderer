MAKEFILE_PATH:=$(abspath $(lastword $(MAKEFILE_LIST)))
ROOT_DIR:=$(realpath $(dir $(MAKEFILE_PATH)))
debug=0
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
endif
CXXFLAGS=-c -std=c++11 -Wall -O2 -I$(ROOT_DIR)/../sr
LDFLAGS=-O2
SOURCES=$(ROOT_DIR)/Application.cpp
ifeq ($(platform),windows)
LDFLAGS+=-u WinMain
SOURCES+=$(ROOT_DIR)/ApplicationWindows.cpp
else ifeq ($(platform),linux)
LDFLAGS+=-lpthread -lX11
SOURCES+=$(ROOT_DIR)/ApplicationLinux.cpp
else ifeq ($(platform),macos)
LDFLAGS+=-framework Cocoa
SOURCES+=$(ROOT_DIR)/ApplicationMacOS.mm
else ifeq ($(platform),ios)
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
LDFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0 \
	-framework UIKit
else ifeq ($(platform),tvos)
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
LDFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0 \
	-framework UIKit
else ifeq ($(platform),emscripten)
LDFLAGS+=--embed-file Resources -s TOTAL_MEMORY=134217728
endif
BASE_NAMES=$(basename $(SOURCES))
OBJECTS=$(BASE_NAMES:=.o)
DEPENDENCIES=$(OBJECTS:.o=.d)
ifeq ($(platform),emscripten)
EXECUTABLE=SoftwareRenderer.js
else
EXECUTABLE=SoftwareRenderer
endif

.PHONY: all
ifeq ($(debug),1)
all: CXXFLAGS+=-DDEBUG -g
endif
all: bundle

.PHONY: bundle
bundle: $(ROOT_DIR)/$(EXECUTABLE)
ifeq ($(platform),macos)
bundle:
	mkdir -p $(ROOT_DIR)/$(EXECUTABLE).app
	mkdir -p $(ROOT_DIR)/$(EXECUTABLE).app/Contents
	sed -e s/'$$(DEVELOPMENT_LANGUAGE)'/en/ \
		-e s/'$$(EXECUTABLE_NAME)'/SoftwareRenderer/ \
		-e s/'$$(PRODUCT_BUNDLE_IDENTIFIER)'/lv.elviss.softwarerenderer/ \
		-e s/'$$(PRODUCT_NAME)'/SoftwareRenderer/ \
		-e s/'$$(MACOSX_DEPLOYMENT_TARGET)'/10.8/ \
		$(ROOT_DIR)/macos/Info.plist > $(ROOT_DIR)/$(EXECUTABLE).app/Contents/Info.plist
	mkdir -p $(ROOT_DIR)/$(EXECUTABLE).app/Contents/MacOS
	cp -f $(ROOT_DIR)/$(EXECUTABLE) $(ROOT_DIR)/$(EXECUTABLE).app/Contents/MacOS
	mkdir -p $(ROOT_DIR)/$(EXECUTABLE).app/Contents/Resources
	cp -f $(ROOT_DIR)/Resources/* $(ROOT_DIR)/$(EXECUTABLE).app/Contents/Resources/
else ifeq ($(platform),ios)
	mkdir -p $(ROOT_DIR)/$(EXECUTABLE).app
	sed -e s/'$$(EXECUTABLE_NAME)'/SoftwareRenderer/ \
		-e s/'$$(PRODUCT_BUNDLE_IDENTIFIER)'/lv.elviss.softwarerenderer/ \
		-e s/'$$(PRODUCT_NAME)'/SoftwareRenderer/ \
		$(ROOT_DIR)/ios/Info.plist > $(ROOT_DIR)/$(EXECUTABLE).app/Info.plist
	cp -f $(ROOT_DIR)/$(EXECUTABLE) $(ROOT_DIR)/$(EXECUTABLE).app
	cp -f $(ROOT_DIR)/Resources/* $(ROOT_DIR)/$(EXECUTABLE).app
else ifeq ($(platform),tvos)
	mkdir -p $(ROOT_DIR)/$(EXECUTABLE).app
	sed -e s/'$$(EXECUTABLE_NAME)'/SoftwareRenderer/ \
		-e s/'$$(PRODUCT_BUNDLE_IDENTIFIER)'/lv.elviss.softwarerenderer/ \
		-e s/'$$(PRODUCT_NAME)'/SoftwareRenderer/ \
		$(ROOT_DIR)/tvos/Info.plist > $(ROOT_DIR)/$(EXECUTABLE).app/Info.plist
	cp -f $(ROOT_DIR)/$(EXECUTABLE) $(ROOT_DIR)/$(EXECUTABLE).app
	cp -f $(ROOT_DIR)/Resources/* $(ROOT_DIR)/$(EXECUTABLE).app
endif

$(ROOT_DIR)/$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

-include $(DEPENDENCIES)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP $< -o $@

%.o: %.mm
	$(CXX) -fno-objc-arc $(CXXFLAGS) -MMD -MP $< -o $@

.PHONY: clean
clean:
ifeq ($(platform),windows)
	-del /f /q "$(ROOT_DIR)\$(EXECUTABLE).exe" "$(ROOT_DIR)\*.o" "$(ROOT_DIR)\*.d" "$(ROOT_DIR)\*.js.mem" "$(ROOT_DIR)\*.js" "$(ROOT_DIR)\*.hpp.gch"
else
	$(RM) $(ROOT_DIR)/$(EXECUTABLE) $(ROOT_DIR)/*.o $(ROOT_DIR)/*.d $(ROOT_DIR)/*.js.mem $(ROOT_DIR)/*.js $(ROOT_DIR)/*.hpp.gch $(ROOT_DIR)/$(EXECUTABLE).exe $(ROOT_DIR)/assetcatalog_generated_info.plist $(ROOT_DIR)/assetcatalog_dependencies
	$(RM) -r $(ROOT_DIR)/$(EXECUTABLE).app
endif