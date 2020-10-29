DEBUG=0
ifeq ($(OS),Windows_NT)
	PLATFORM=windows
else
	architecture=$(shell uname -m)
	os=$(shell uname -s)

	ifeq ($(os),Linux)
		PLATFORM=linux
	else ifeq ($(os),SunOS)
		PLATFORM=sunos
	else ifeq ($(os),FreeBSD)
		PLATFORM=bsd
	else ifeq ($(os),DragonFly)
		PLATFORM=bsd
	else ifeq ($(os),NetBSD)
		PLATFORM=bsd
	else ifeq ($(os),Darwin)
		PLATFORM=macos
	else ifeq ($(os),Haiku)
		PLATFORM=haiku
	endif
endif
ifeq ($(PLATFORM),emscripten)
CC=emcc
CXX=em++
endif
CXXFLAGS=-std=c++11 -Wall -O2 -I../sr
LDFLAGS=-O2
SOURCES=Application.cpp
ifeq ($(PLATFORM),windows)
LDFLAGS+=-u WinMain
SOURCES+=ApplicationWindows.cpp
else ifeq ($(PLATFORM),linux)
LDFLAGS+=-lX11
SOURCES+=ApplicationX11.cpp
else ifeq ($(PLATFORM),sunos)
LDFLAGS+=-lX11
SOURCES+=ApplicationX11.cpp
else ifeq ($(PLATFORM),bsd)
CXXFLAGS+=-I/usr/local/include
LDFLAGS+=-lX11 -L/usr/local/lib
SOURCES+=ApplicationX11.cpp
else ifeq ($(PLATFORM),macos)
LDFLAGS+=-framework Cocoa
SOURCES+=ApplicationMacOS.mm
else ifeq ($(PLATFORM),haiku)
LDFLAGS+=-lbe
SOURCES+=ApplicationHaiku.cpp
else ifeq ($(PLATFORM),ios)
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
LDFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0 \
	-framework CoreGraphics -framework Foundation -framework UIKit
SOURCES+=ApplicationIOS.mm
else ifeq ($(PLATFORM),tvos)
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
LDFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0 \
	-framework CoreGraphics -framework Foundation -framework UIKit
SOURCES+=ApplicationTVOS.mm
else ifeq ($(PLATFORM),emscripten)
LDFLAGS+=--embed-file Resources -s TOTAL_MEMORY=134217728
endif
BASE_NAMES=$(basename $(SOURCES))
OBJECTS=$(BASE_NAMES:=.o)
DEPENDENCIES=$(OBJECTS:.o=.d)
ifeq ($(PLATFORM),emscripten)
EXECUTABLE=SoftwareRenderer.js
else
EXECUTABLE=SoftwareRenderer
endif

.PHONY: all
ifeq ($(DEBUG),1)
all: CXXFLAGS+=-DDEBUG -g
endif
all: bundle

.PHONY: bundle
bundle: $(EXECUTABLE)
ifeq ($(PLATFORM),macos)
bundle:
	mkdir -p $(EXECUTABLE).app
	mkdir -p $(EXECUTABLE).app/Contents
	sed -e s/'$$(DEVELOPMENT_LANGUAGE)'/en/ \
		-e s/'$$(EXECUTABLE_NAME)'/SoftwareRenderer/ \
		-e s/'$$(PRODUCT_BUNDLE_IDENTIFIER)'/lv.elviss.softwarerenderer/ \
		-e s/'$$(PRODUCT_NAME)'/SoftwareRenderer/ \
		-e s/'$$(MACOSX_DEPLOYMENT_TARGET)'/10.8/ \
		macos/Info.plist > $(EXECUTABLE).app/Contents/Info.plist
	mkdir -p $(EXECUTABLE).app/Contents/MacOS
	cp -f $(EXECUTABLE) $(EXECUTABLE).app/Contents/MacOS
	mkdir -p $(EXECUTABLE).app/Contents/Resources
	cp -f Resources/* $(EXECUTABLE).app/Contents/Resources/
else ifeq ($(PLATFORM),ios)
	mkdir -p $(EXECUTABLE).app
	sed -e s/'$$(EXECUTABLE_NAME)'/SoftwareRenderer/ \
		-e s/'$$(PRODUCT_BUNDLE_IDENTIFIER)'/lv.elviss.softwarerenderer/ \
		-e s/'$$(PRODUCT_NAME)'/SoftwareRenderer/ \
		ios/Info.plist > $(EXECUTABLE).app/Info.plist
	cp -f $(EXECUTABLE) $(EXECUTABLE).app
	cp -f Resources/* $(EXECUTABLE).app
else ifeq ($(PLATFORM),tvos)
	mkdir -p $(EXECUTABLE).app
	sed -e s/'$$(EXECUTABLE_NAME)'/SoftwareRenderer/ \
		-e s/'$$(PRODUCT_BUNDLE_IDENTIFIER)'/lv.elviss.softwarerenderer/ \
		-e s/'$$(PRODUCT_NAME)'/SoftwareRenderer/ \
		tvos/Info.plist > $(EXECUTABLE).app/Info.plist
	cp -f $(EXECUTABLE) $(EXECUTABLE).app
	cp -f Resources/* $(EXECUTABLE).app
endif

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

-include $(DEPENDENCIES)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -MMD -MP $< -o $@

%.o: %.mm
	$(CXX) -c -fno-objc-arc $(CXXFLAGS) -MMD -MP $< -o $@

.PHONY: clean
clean:
ifeq ($(PLATFORM),windows)
	-del /f /q "$(EXECUTABLE).exe" "*.o" "*.d" "*.js.mem" "*.js" "*.hpp.gch"
else
	$(RM) $(EXECUTABLE) *.o *.d *.js.mem *.js *.hpp.gch $(EXECUTABLE).exe assetcatalog_generated_info.plist assetcatalog_dependencies
	$(RM) -r $(EXECUTABLE).app
endif