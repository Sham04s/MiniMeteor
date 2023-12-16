# Inputs
RAYLIB_PATH 				?= ../raylib
BUILD_MODE 					?= RELEASE
HOT_RELOAD 					?= FALSE
MAIN_SRC_FILES 				?= src/main.cpp
CORE_SRC_FILES 				?= $(filter-out $(MAIN_SRC_FILES), $(wildcard src/*.cpp))
PLATFORM 					?= PLATFORM_DESKTOP
PROJECT_BUILD_DIR 			?= ./build
PROJECT_NAME 				?= MiniMeteor

# PLATFORM_WEB: Default properties
BUILD_WEB_ASYNCIFY    		?= FALSE
BUILD_WEB_SHELL       		?= src/minshell.html
BUILD_WEB_HEAP_SIZE   		?= 128MB
BUILD_WEB_RESOURCES   		?= TRUE
BUILD_WEB_RESOURCES_PATH  	?= resources

CORE_OBJS := $(CORE_SRC_FILES:.cpp=.o)
MAIN_OBJS := $(MAIN_SRC_FILES:.cpp=.o)

ifeq ($(OS),Windows_NT)
	PLATFORM_OS := WINDOWS
else
	PLATFORM_OS := $(shell uname -s)
	ifeq ($(PLATFORM_OS),Linux)
		PLATFORM_OS := LINUX
	endif
endif

# Defines
DFLAGS := -D$(PLATFORM)

# Include directories
INCLUDES := -I./include -I$(RAYLIB_PATH)/src

# Library directories
LDFLAGS := -L$(RAYLIB_PATH)/src

# Compiler flags
CXXFLAGS := -Wall -Werror -Wextra -std=c++20 -Wno-missing-field-initializers

ifeq ($(BUILD_MODE), DEBUG)
	CXXFLAGS += -g -O0
	DFLAGS += -D_DEBUG
else
	ifeq ($(PLATFORM), PLATFORM_WEB)
		ifeq ($(BUILD_WEB_ASYNCIFY),TRUE)
            CXXFLAGS += -O3
        else
            CXXFLAGS += -Os
        endif
	else
		CXXFLAGS += -s -O2
	endif
endif

# for hot reloading on windows
ifeq ($(HOT_RELOAD), TRUE)
	ifeq ($(PLATFORM), PLATFORM_DESKTOP)
		ifeq ($(PLATFORM_OS), WINDOWS)
			CORE_LIB = $(PROJECT_BUILD_DIR)/core.dll
			DFLAGS += -DWINDOWS_HOT_RELOAD
# 		link raylib as a shared library
			LDLIBS = -l:raylib.dll
		endif
	endif
else
	MAIN_OBJS += $(CORE_OBJS)
	LDLIBS = -lraylib
endif

# Specific libraries for each platform
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
	ifeq ($(PLATFORM_OS), WINDOWS)
		LDLIBS += -lopengl32 -lgdi32 -lwinmm
	endif
	ifeq ($(PLATFORM_OS), LINUX)
		LDLIBS += -lGL -lm -lpthread -ldl -lrt -lX11
	endif
else
	ifeq ($(PLATFORM),PLATFORM_WEB)
		LDLIBS = $(RAYLIB_PATH)/src/libraylib.a
	endif
endif

# C++ Compiler
CXX := g++

# if compiling for web, use emscripten
ifeq ($(PLATFORM), PLATFORM_WEB)
	CXX := emcc
endif

ifeq ($(PLATFORM),PLATFORM_WEB)
    # -Os                        # size optimization
    # -O2                        # optimization level 2, if used, also set --memory-init-file 0
    # -s USE_GLFW=3              # Use glfw3 library (context/input management)
    # -s ALLOW_MEMORY_GROWTH=1   # to allow memory resizing -> WARNING: Audio buffers could FAIL!
    # -s TOTAL_MEMORY=16777216   # to specify heap memory size (default = 16MB) (67108864 = 64MB)
    # -s USE_PTHREADS=1          # multithreading support
    # -s WASM=0                  # disable Web Assembly, emitted by default
    # -s ASYNCIFY                # lets synchronous C/C++ code interact with asynchronous JS
    # -s FORCE_FILESYSTEM=1      # force filesystem to load/save files data
    # -s ASSERTIONS=1            # enable runtime checks for common memory allocation errors (-O1 and above turn it off)
    # --profiling                # include information for code profiling
    # --memory-init-file 0       # to avoid an external memory initialization code file (.mem)
    # --preload-file resources   # specify a resources folder for data compilation
    # --source-map-base          # allow debugging in browser with source map
    LDFLAGS += -s USE_GLFW=3 -s TOTAL_MEMORY=$(BUILD_WEB_HEAP_SIZE) -s FORCE_FILESYSTEM=1
    
    # Build using asyncify
    ifeq ($(BUILD_WEB_ASYNCIFY),TRUE)
        LDFLAGS += -s ASYNCIFY
    endif
    
    # Add resources building if required
    ifeq ($(BUILD_WEB_RESOURCES),TRUE)
        LDFLAGS += --preload-file $(BUILD_WEB_RESOURCES_PATH)
    endif
    
    # Add debug mode flags if required
    ifeq ($(BUILD_MODE),DEBUG)
        LDFLAGS += -s ASSERTIONS=1 --profiling
    endif

    # Define a custom shell .html and output extension
    LDFLAGS += --shell-file $(BUILD_WEB_SHELL)
    EXT = .html
endif

# -Wl,--subsystem,windows hides the console window
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
	ifeq ($(PLATFORM_OS),WINDOWS)		
		EXT = .exe
		ifeq ($(BUILD_MODE), RELEASE)	
			LDFLAGS += -Wl,--subsystem,windows
		endif
	endif
endif

# Executable name
EXECUTABLE = $(PROJECT_BUILD_DIR)/$(PROJECT_NAME)$(EXT)

vpath %.cpp src

.PHONY: all clean

all: $(EXECUTABLE) $(CORE_LIB)

# Rule to build executable
$(EXECUTABLE): $(MAIN_OBJS)
	mkdir -p $(PROJECT_BUILD_DIR)
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
	mkdir -p $(PROJECT_BUILD_DIR)/resources
	cp -r resources/* $(PROJECT_BUILD_DIR)/resources
endif
	$(CXX) -o $(EXECUTABLE) $(MAIN_OBJS) $(LDFLAGS) $(LDLIBS)

# Rule to build core as a shared library (dll)
$(PROJECT_BUILD_DIR)/core.dll: $(CORE_OBJS)
	$(CXX) -shared -o $(PROJECT_BUILD_DIR)/core.dll $^ $(LDFLAGS) -l:raylib.dll

# Rule to build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DFLAGS) -c $< -o $@

# Copy resources folder (only for desktop platforms)
res:
	mkdir -p $(PROJECT_BUILD_DIR)/resources
	cp -r resources/* $(PROJECT_BUILD_DIR)/resources

# Clean rule
clean:
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
ifeq ($(PLATFORM_OS),WINDOWS)
	rm -f $(EXECUTABLE) $(PROJECT_BUILD_DIR)/core.dll
endif
	rm -rf $(PROJECT_BUILD_DIR)/resources
endif
ifeq ($(PLATFORM),PLATFORM_WEB)
	rm -f $(EXECUTABLE) $(PROJECT_BUILD_DIR)/*.js $(PROJECT_BUILD_DIR)/*.wasm $(PROJECT_BUILD_DIR)/*.data
endif
	rm -rf $(PROJECT_BUILD_DIR)
	rm src/*.o
