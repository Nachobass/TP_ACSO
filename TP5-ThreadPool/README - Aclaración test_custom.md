**ACLARACIÓN:**

Si quieren correr el test_custom tienen que modificar el Make file y usar este:



# Compiler settings - Can change to clang++ if preferred
CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread -g

# Build targets
TARGET = threadpool
SRC = thread-pool.cc Semaphore.cc test_more.cc

# Link the target with object files
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^


custom:
	$(CXX) $(CXXFLAGS) -o $(TARGET) thread-pool.cc Semaphore.cc test_custom.cc
	
# Clean up build artifacts
clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean
