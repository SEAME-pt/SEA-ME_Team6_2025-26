CXX ?= g++

BIN_DIR := bin

CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -I.

ifdef SDKTARGETSYSROOT
  CXXFLAGS += --sysroot=$(SDKTARGETSYSROOT)
  CXXFLAGS += -I$(SDKTARGETSYSROOT)/usr/include
  LDFLAGS  += --sysroot=$(SDKTARGETSYSROOT)
  LDFLAGS  += -L$(SDKTARGETSYSROOT)/usr/lib
  LDFLAGS  += -Wl,-rpath-link,$(SDKTARGETSYSROOT)/usr/lib
endif

LDFLAGS += -lpthread

TARGET := $(BIN_DIR)/adas_manager
SRC    := adas_manager.cpp
HDRS   := $(wildcard *.hpp) $(wildcard *.h)

all: $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(SRC) $(HDRS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)
	@echo "=== Binary info ==="
	@file $(TARGET)

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean
