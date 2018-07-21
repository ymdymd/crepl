# target
# カレントディレクト名をターゲット名称にする。
TARGET	?= $(notdir $(CURDIR))
 
# build type
BUILD_TYPE ?= Debug 

# directory
BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
 
SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
 
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
 
CPPFLAGS ?= $(INC_FLAGS) -MMD -MP


CC := clang 
CFLAGS	?= -Wall -Wextra

CXX := clang++
CXXFLAGS ?=  -Wall -Wextra -std=c++14
LDFLAGS +=   -lstdc++  -ledit -ltermcap 


ifeq ($(BUILD_TYPE),Debug)
CFLAGS	+= -g -O0
CXXFLAGS	+= -g -O0
else
CFLAGS	+= -O3
CXXFLAGS	+= -O3
endif


$(TARGET): $(OBJS)
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
	$(RM) -r $(TARGET) $(BUILD_DIR) .history

.PHONY: run
run: $(TARGET)
	./$(TARGET)

.PHONY: format
format:	$(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.h -or -name *.hpp)
	clang-format -i $^

.PHONY: test
test: FORCE
	make -C test run

.PHONY: FORCE
FORCE:

-include $(DEPS)
 
MKDIR_P ?= mkdir -p
