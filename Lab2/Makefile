CXX       := g++
WINDRES   := windres
SRC_DIR   := src
RES_DIR   := res
BUILD_DIR := build

CXXFLAGS  := -std=c++17 -municode -DUNICODE -D_UNICODE -Wall -Wextra -I$(SRC_DIR)
LDFLAGS   := -municode -mwindows
LDLIBS    := -luser32 -lgdi32

TARGET    := Lab2.exe
SOURCES   := $(SRC_DIR)/Lab2.cpp $(SRC_DIR)/shape.cpp $(SRC_DIR)/shape_editor.cpp
OBJECTS   := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
RES_OBJ   := $(BUILD_DIR)/resource.res.o
HEADERS   := $(SRC_DIR)/resource.h $(SRC_DIR)/shape.h $(SRC_DIR)/editor.h $(SRC_DIR)/shape_editor.h

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) $(RES_OBJ)
	$(CXX) $(OBJECTS) $(RES_OBJ) -o $@ $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(RES_OBJ): $(RES_DIR)/resource.rc $(SRC_DIR)/resource.h | $(BUILD_DIR)
	$(WINDRES) -I$(SRC_DIR) $< -O coff -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)