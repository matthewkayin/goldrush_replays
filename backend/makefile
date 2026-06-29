# ------------------------------------------------------------------------------
# Project
# ------------------------------------------------------------------------------

TARGET          := backend
BUILD_DIR       := bin
OBJ_DIR         := obj
SRC_DIR         := src
LIB_DIR         := lib

CXX             := clang++
CXXSTD          := -std=c++17
C             := clang
CSTD          := -std=c99

INCLUDE_DIRS    := -Isrc -Ivendor
DEFINES         := -D_CRT_SECURE_NO_WARNINGS

CXXFLAGS        := $(CXXSTD)
LDFLAGS         := $(CXXSTD)

EXTENSION       :=

# ------------------------------------------------------------------------------
# Build Configuration
# ------------------------------------------------------------------------------

ifeq ($(RELEASE),)
	CXXFLAGS += -O0 -g -Wall -Wextra -Wshadow
	LDFLAGS  += -g
else
	CXXFLAGS += -O2
endif

ifeq ($(profiler),enabled)
	CXXFLAGS += -Ivendor/tracy
	DEFINES  += -DTRACY_ENABLE
endif

# ------------------------------------------------------------------------------
# Platform Detection
# ------------------------------------------------------------------------------

ifeq ($(OS),Windows_NT)

	PLATFORM   := win64
	EXTENSION  := .exe
	LIB_DIR    := lib\win64

	# Recursive wildcard
	rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
	SRC_FILES   := $(call rwildcard,$(SRC_DIR)/,*.cpp)
	SRC_FILES   += $(call rwildcard,$(SRC_DIR)/,*.c)
	RC_FILES    := $(SRC_DIR)/win_icon.rc
	DIRECTORY   := $(subst /,\,${CURDIR})
	SHELL       := cmd
	DIRECTORIES := \$(SRC_DIR) $(subst $(DIRECTORY),,$(shell dir $(SRC_DIR) /S /AD /B | findstr /i $(SRC_DIR)))

	DEFINES += -D_WIN32_WINNT=0x0A00

else

	UNAME_S := $(shell uname -s)

	SRC_FILES := $(shell find $(SRC_DIR) -type f \( -name "*.cpp" -o -name "*.mm" -o -name "*.c" \))
	DIRECTORIES := $(shell find src -type d)

	ifeq ($(UNAME_S),Darwin)

		PLATFORM := macos
		LIB_DIR  := lib/macos
		CXXFLAGS += -Wno-deprecated-declarations

		ifeq ($(RELEASE_VERSION),)
			LDFLAGS += -rpath ../$(LIB_DIR)
		endif

	endif

	ifeq ($(UNAME_S),Linux)

		PLATFORM := linux
		LIB_DIR  := lib/linux64

	endif

endif

# ------------------------------------------------------------------------------
# Source/Object Lists
# ------------------------------------------------------------------------------

OBJ_FILES := $(SRC_FILES:%=$(OBJ_DIR)/%.o)

# ------------------------------------------------------------------------------
# Default Target
# ------------------------------------------------------------------------------

.PHONY: all
all: scaffold compile link

# ------------------------------------------------------------------------------
# Directory Setup
# ------------------------------------------------------------------------------

.PHONY: scaffold
scaffold:
	@echo Scaffolding...
ifeq ($(PLATFORM),win64)
	@mkdir $(addprefix $(OBJ_DIR), $(DIRECTORIES)) 2>NUL || cd .
	@mkdir $(BUILD_DIR) 2>NUL || cd .
else
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(addprefix $(OBJ_DIR)/,$(DIRECTORIES))
endif
	@echo Done.

# ------------------------------------------------------------------------------
# Compile
# ------------------------------------------------------------------------------

# This phony just prints messages before compilation
.PHONY: compile
compile:
	@echo Compiler flags: $(CXXFLAGS)
	@echo Defines:       $(DEFINES)
	@echo Compiling...

# Compile cpp to object
$(OBJ_DIR)/%.cpp.o: %.cpp
	@echo   $<...
	@$(CXX) $< $(CXXFLAGS) -c -o $@ $(DEFINES) $(INCLUDE_DIRS)

# Compile c to object
$(OBJ_DIR)/%.c.o: %.c
	@echo   $<...
	@$(C) $< $(CFLAGS) -c -o $@ $(DEFINES) $(INCLUDE_DIRS)

# ------------------------------------------------------------------------------
# Link
# ------------------------------------------------------------------------------

.PHONY: link
link: scaffold $(OBJ_FILES)
	@echo Linker flags $(LINKER_FLAGS)
	@echo Linking $(ASSEMBLY)...
ifeq ($(PLATFORM),win64)
	@$(CXX) $(OBJ_FILES) -o $(BUILD_DIR)\$(TARGET)$(EXTENSION) $(LDFLAGS)
else
	@$(CXX) $(OBJ_FILES) -o $(BUILD_DIR)/$(TARGET)$(EXTENSION) $(LDFLAGS)
endif

# ------------------------------------------------------------------------------
# Clean
# ------------------------------------------------------------------------------

.PHONY: clean
clean:
	@echo Cleaning...

ifeq ($(PLATFORM),win64)
	@if exist $(BUILD_DIR)\$(TARGET)$(EXTENSION) del $(BUILD_DIR)\$(TARGET)$(EXTENSION)
	@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)
else
	@rm -rf $(OBJ_DIR)
	@rm -f $(BUILD_DIR)/$(TARGET)$(EXTENSION)
endif

ifeq ($(PLATFORM),macos)
	@rm -rf $(BUILD_DIR)/Gold\ Rush.app
endif

# ------------------------------------------------------------------------------
# Utility Targets
# ------------------------------------------------------------------------------

.PHONY: libcopy
libcopy:
ifeq ($(PLATFORM),win64)
	-@setlocal enableextensions enabledelayedexpansion && xcopy $(LIB_DIR) $(BUILD_DIR) /Y
endif
ifeq ($(PLATFORM),macos)
	-@cp $(LIB_DIR)/*.a $(BUILD_DIR)/
	-@cp $(LIB_DIR)/*.dylib $(BUILD_DIR)/
endif
ifeq ($(PLATFORM),linux)
	-@cp $(LIB_DIR)/* $(BUILD_DIR)/
endif

.PHONY: luadoc
luadoc:
ifeq ($(PLATFORM),win64)
	@cd $(BUILD_DIR) && $(TARGET)$(EXTENSION) --lua-doc
else
	@cd $(BUILD_DIR) && ./$(TARGET)$(EXTENSION) --lua-doc
endif
