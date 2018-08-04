# target
# カレントディレクト名をターゲット名称にする。
TARGET	?= $(notdir $(CURDIR))

# switch coverage build
COV ?= 1 

# switch debug build
#DEBUG ?= 1 

# force debgg flag at coverage mode
ifdef COV
DEBUG := 1
endif


# directory
BUILD_DIR := ../build
SRC_DIR := ../src
GTEST_DIR := ../googletest/googletest
COV_DIR := ./coverage

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

GCOV := "llvm-cov gcov"
GCOVFLAGS :=
GCOVFLAGS += --print-summary
#GCOVFLAGS += --exclude-unreachable-branches	#bug??

# debug 
ifdef DEBUG
CFLAGS	+= -g -O0 -DDEBUG
CXXFLAGS	+= -g -O0 -DDEBUG
# release 
else
CFLAGS	+= -O3
CXXFLAGS	+= -O3
endif

# coverage 
ifdef COV
CFLAGS	+= --coverage
CXXFLAGS	+= --coverage
LDFLAGS +=   --coverage
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
	$(RM) -r $(OBJS) $(DEPS) $(TARGET) *.gcov *.gcno *.gcda $(COV_DIR)

.PHONY: run
run: $(TARGET)
	./$(TARGET)

.PHONY: format
format:	$(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.h -or -name *.hpp)
	clang-format -i $^

.PHONY: cov
cov: run
	$(MKDIR_P) $(COV_DIR)
	gcovr --gcov-executable=$(GCOV) -r . --filter $(SRC_DIR)  $(GCOVFLAGS) --html --html-details -o $(COV_DIR)/index.html

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
