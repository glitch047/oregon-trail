CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LDFLAGS = 
INCLUDES = -I.
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Executable
EXECUTABLE = $(BIN_DIR)/oregon_trail

# Default target
all: directories $(EXECUTABLE)

# Create build directories
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

# Link the executable
$(EXECUTABLE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS) $(LIBS)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run the game
run: all
	$(EXECUTABLE)

.PHONY: all directories clean run

