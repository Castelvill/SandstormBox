CXX := g++
CXXFLAGS := -fdiagnostics-color=always -g -Wall -std=c++20

# Define the Allegro libraries
LIBS := -lallegro -lallegro_font -lallegro_ttf -lallegro_image -lallegro_primitives -lallegro_dialog -static-libasan

SRC_DIR := src
SRC := $(SRC_DIR)/vectors.cpp $(SRC_DIR)/usefull.cpp $(SRC_DIR)/primaryModule.cpp $(SRC_DIR)/collisionModule.cpp \
       $(SRC_DIR)/movementModule.cpp $(SRC_DIR)/variableModule.cpp $(SRC_DIR)/camera.cpp $(SRC_DIR)/primitivesModule.cpp \
       $(SRC_DIR)/mouse.cpp $(SRC_DIR)/imageModule.cpp $(SRC_DIR)/scrollbarModule.cpp $(SRC_DIR)/particles.cpp \
       $(SRC_DIR)/textModule.cpp $(SRC_DIR)/superTextModule.cpp $(SRC_DIR)/vectorModule.cpp $(SRC_DIR)/eventModule.cpp \
       $(SRC_DIR)/ancestorObject.cpp $(SRC_DIR)/layerClass.cpp $(SRC_DIR)/processClass.cpp $(SRC_DIR)/engineClass.cpp \
       $(SRC_DIR)/main.cpp

OUTPUT := SandstormBox

# Default target
all: $(OUTPUT)

# Link all object files to create the executable
$(OUTPUT): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OUTPUT) $(SRC) $(LIBS)

# Clean target to remove the executable
clean:
	rm -f $(OUTPUT)
