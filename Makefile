# https://spin.atomicobject.com/makefile-c-projects/
TARGET_EXEC ?= rl3d

UNAME:=$(shell uname)

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS += inc

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
CPPFLAGS += -Wall -Werror -Wno-comment
CPPFLAGS += -g

ifeq ($(OS),Windows_NT)
  LDFLAGS += -Llib/win -lraylib -lgdi32 -lwinmm
else
  LDFLAGS += -Llib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	@echo we are on system $(UNAME)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p