# target
# カレントディレクト名をターゲット名称にする。
TARGET	?= $(notdir $(CURDIR))
 
# build type
BUILD_TYPE ?= Debug 

# directory
BUILD_DIR := ../build
SRC_DIR := ../src
GTEST_DIR := ../googletest/googletest


SRCS := $(SRC_DIR)/expr.cpp
SRCS += $(GTEST_DIR)/src/gtest-all.cc
SRCS += main.cpp

VPATH := $(SRC_DIR)
VPATH += $(GTEST_DIR)/src

OBJS :=
OBJS += $(patsubst %.cpp,%.o,$(filter %.cpp ,$(notdir $(SRCS))))
OBJS += $(patsubst %.cc,%.o,$(filter %.cc ,$(notdir $(SRCS))))
DEPS := $(OBJS:.o=.d)

INC_DIRS :=
INC_DIRS += $(SRC_DIR)
INC_DIRS += $(GTEST_DIR)
INC_DIRS += $(GTEST_DIR)/include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
 
CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

CC := clang 
CFLAGS	?= -Wall

CXX := clang++
CXXFLAGS ?=  -Wall -std=c++14
LDFLAGS +=   -lstdc++ -lpthread


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
%.o: %.s
	$(AS) $(ASFLAGS) -c $< -o $@
 
# c source
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
 
# c++ source
%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# c++ source
%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
 
clean:
	$(RM) -r $(OBJS) $(DEPS) $(TARGET)

.PHONY: run
 
run:
	./$(TARGET)

.PHONY: format
 
format:	$(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.h -or -name *.hpp)
	clang-format -i $^

.PHONY: debug
debug:
	@echo "SRC_DIR:" $(SRC_DIR)
	@echo "GTEST_DIR:" $(GTEST_DIR)
	@echo "OBJS:" $(OBJS)
	@echo "INC_DIRS:" $(INC_DIRS)
	@echo "INC_FLAGS:" $(INC_FLAGS)
	@echo "VPASS:" $(VPATH)

-include $(DEPS)
 
MKDIR_P ?= mkdir -p
