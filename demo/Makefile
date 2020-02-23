debug=0
ifeq ($(OS),Windows_NT)
	platform=windows
else
	architecture=$(shell uname -m)
	os=$(shell uname -s)

	ifeq ($(os),Linux)
		platform=linux
	else ifeq ($(os),SunOS)
		platform=sunos
	else ifeq ($(os),FreeBSD)
		platform=bsd
	else ifeq ($(os),DragonFly)
		platform=bsd
	else ifeq ($(os),NetBSD)
		platform=bsd
	else ifeq ($(os),Darwin)
		platform=macos
	else ifeq ($(os),Haiku)
		platform=haiku
	endif
endif
ifeq ($(platform),emscripten)
CC=emcc
CXX=em++
endif
CXXFLAGS=-c -std=c++11 -Wall -O2 -I../sr
LDFLAGS=-O2
SOURCES=Application.cpp
ifeq ($(platform),windows)
LDFLAGS+=-u WinMain
SOURCES+=ApplicationWindows.cpp
else ifeq ($(platform),linux)
LDFLAGS+=-lX11
SOURCES+=ApplicationX11.cpp
else ifeq ($(platform),sunos)
LDFLAGS+=-lX11
SOURCES+=ApplicationX11.cpp
else ifeq ($(platform),bsd)
CXXFLAGS+=-I/usr/local/include
LDFLAGS+=-lX11 -L/usr/local/lib
SOURCES+=ApplicationX11.cpp
else ifeq ($(platform),macos)
LDFLAGS+=-framework Cocoa
SOURCES+=ApplicationMacOS.mm
else ifeq ($(platform),haiku)
LDFLAGS+=-lbe
SOURCES+=ApplicationHaiku.cpp
else ifeq ($(platform),ios)
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
LDFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0 \
	-framework CoreGraphics -framework Foundation -framework UIKit
SOURCES+=ApplicationIOS.mm
else ifeq ($(platform),tvos)
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
LDFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0 \
	-framework CoreGraphics -framework Foundation -framework UIKit
SOURCES+=ApplicationTVOS.mm
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
bundle: $(EXECUTABLE)
ifeq ($(platform),macos)
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
else ifeq ($(platform),ios)
	mkdir -p $(EXECUTABLE).app
	sed -e s/'$$(EXECUTABLE_NAME)'/SoftwareRenderer/ \
		-e s/'$$(PRODUCT_BUNDLE_IDENTIFIER)'/lv.elviss.softwarerenderer/ \
		-e s/'$$(PRODUCT_NAME)'/SoftwareRenderer/ \
		ios/Info.plist > $(EXECUTABLE).app/Info.plist
	cp -f $(EXECUTABLE) $(EXECUTABLE).app
	cp -f Resources/* $(EXECUTABLE).app
else ifeq ($(platform),tvos)
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
	$(CXX) $(CXXFLAGS) -MMD -MP $< -o $@

%.o: %.mm
	$(CXX) -fno-objc-arc $(CXXFLAGS) -MMD -MP $< -o $@

.PHONY: clean
clean:
ifeq ($(platform),windows)
	-del /f /q "$(EXECUTABLE).exe" "*.o" "*.d" "*.js.mem" "*.js" "*.hpp.gch"
else
	$(RM) $(EXECUTABLE) *.o *.d *.js.mem *.js *.hpp.gch $(EXECUTABLE).exe assetcatalog_generated_info.plist assetcatalog_dependencies
	$(RM) -r $(EXECUTABLE).app
endif