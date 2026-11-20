CXX      := g++
WINDRES  := windres
TARGET   := Lab5.exe

SRC_DIR   := src
RES_DIR   := res
BUILD_DIR := build

CXXFLAGS := -std=c++17 -DUNICODE -D_UNICODE -Wall -Wextra -O2 -I$(SRC_DIR)
LDFLAGS  := -municode -mwindows -static -static-libgcc -static-libstdc++
LIBS     := -lcomctl32 -lcomdlg32 -lgdi32 -luser32

SOURCES := $(SRC_DIR)/Lab5.cpp $(SRC_DIR)/shape.cpp $(SRC_DIR)/my_editor.cpp $(SRC_DIR)/my_table.cpp
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
RES_OBJ := $(BUILD_DIR)/resource.o

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS) $(RES_OBJ)
	$(CXX) $(OBJECTS) $(RES_OBJ) -o $@ $(LDFLAGS) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(RES_OBJ): $(RES_DIR)/resource.rc $(SRC_DIR)/resource.h | $(BUILD_DIR)
	$(WINDRES) -I$(SRC_DIR) $(RES_DIR)/resource.rc -O coff -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean