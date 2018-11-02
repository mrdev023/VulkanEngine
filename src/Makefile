OUTPUT_NAME = main

SRC_DIR := .
OBJ_DIR := ../obj
OUTPUT_DIR := ../

CXX := g++

CXX_FLAGS := -O3 -Wall
LD_FLAGS = -lglfw -lvulkan

CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)
CPPOBJ_FILES := $(addprefix $(OBJ_DIR)/,$(notdir $(CPP_FILES:.cpp=.o)))

main: $(CPPOBJ_FILES)
	$(CXX) -o $(OUTPUT_DIR)/$(OUTPUT_NAME) $(CPPOBJ_FILES) $(LD_FLAGS)
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@ -g

clean:
	rm -f $(OBJ_DIR)/*
	rm -f $(OUTPUT_DIR)/$(OUTPUT_NAME)