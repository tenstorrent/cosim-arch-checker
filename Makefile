# Compiler settings
CC = g++
CFLAGS = -std=c++17 -Wall -Werror -Wno-unused-private-field -fpic -Imon/mon_instr -Ibridge/std -Ibridge -Ienv -Ibridge/whisper/svdpi -Ibridge/whisper -Icac/src/lib -Icac/src
LDLIBS = 

# Directories
SRC_DIR = .
BUILD_DIR = build
LIB_DIR = lib

# Source files
SRCS = $(shell find . -name '*.cpp' -o -name '*.cxx' -o -name '*.cc')
$(info SRCS=$(SRCS))
OBJS = $(patsubst $(SRC_DIR)/%.cc, $(BUILD_DIR)/%.o, $(patsubst $(SRC_DIR)/%.cxx, $(BUILD_DIR)/%.o, $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))))
$(info OBJS=$(OBJS))
DEPS = $(OBJS:.o=.d)

# Library name and version
LIB_NAME = cosim

# Targets
.PHONY: all clean

all: $(LIB_DIR)/lib$(LIB_NAME).so

clean:
	rm -rf $(BUILD_DIR)/* $(LIB_DIR)/*

$(LIB_DIR)/lib$(LIB_NAME).so: $(OBJS)
	$(CC) $(LDLIBS) -shared -Wl,-soname,lib$(LIB_NAME).so -o $@ $^

-include $(DEPS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cxx
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<
