SRC_DIR = src
OBJ_DIR = obj
SRC = $(wildcard $(SRC_DIR)/*.cpp)

CXX = g++
CPPFLAGS += -Iinclude #include directory
CXXFLAGS = -g --std=c++11 -Wall

all: freq 

# Frequency Analysis executable
FREQ_OBJ = $(addprefix $(OBJ_DIR)/, freq.o)
freq: directories $(FREQ_OBJ)
	$(CXX) -o $@ $(FREQ_OBJ)

# Object folder targets:
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# phonies
.PHONY: clean fresh directories

directories: $(OBJ_DIR)

clean:
	rm -rf obj/

fresh: clean
	rm -f freq
