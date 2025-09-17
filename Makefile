# Compiler
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -g

# Target executable
TARGET = Relational_Algebra

# Source file
SRC = Relational_Algebra.c++

# Build rule
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Run rule
run: $(TARGET)
	./$(TARGET)

# Clean rule
clean:
	rm -f $(TARGET)
