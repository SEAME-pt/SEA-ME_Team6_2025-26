CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
CXXFLAGS += -Iinc -Igenerated
CXXFLAGS += $(shell pkg-config --cflags grpc++ protobuf 2>/dev/null)

LDFLAGS := $(shell pkg-config --libs grpc++ protobuf 2>/dev/null)
LDFLAGS := $(filter-out -lutf8_validity -lutf8_range,$(LDFLAGS))
LDFLAGS += -lpthread

BIN_DIR := bin
SRC_DIR := src
GEN_DIR := generated
HANDLERS_DIR := $(SRC_DIR)/handlers

PUBLISHER_BIN := $(BIN_DIR)/can_to_kuksa_publisher
READER_BIN    := $(BIN_DIR)/kuksa_reader

GEN_SRCS := \
  $(GEN_DIR)/kuksa/val/v2/types.pb.cc \
  $(GEN_DIR)/kuksa/val/v2/types.grpc.pb.cc \
  $(GEN_DIR)/kuksa/val/v2/val.pb.cc \
  $(GEN_DIR)/kuksa/val/v2/val.grpc.pb.cc

# --- Publisher sources (new structure) ---
PUBLISHER_SRCS := \
  $(SRC_DIR)/main.cpp \
  $(SRC_DIR)/dispatch_frames.cpp \
  $(SRC_DIR)/kuksa_client.cpp \
  $(HANDLERS_DIR)/speed.cpp \
  $(HANDLERS_DIR)/environment.cpp \
  $(HANDLERS_DIR)/heartbeat_stm.cpp \
  $(GEN_SRCS)

# --- Reader sources ---
READER_SRCS := \
  $(SRC_DIR)/kuksa_reader.cpp \
  $(GEN_SRCS)

# all: $(PUBLISHER_BIN) $(READER_BIN) not needed reader
all: $(PUBLISHER_BIN)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(PUBLISHER_BIN): $(PUBLISHER_SRCS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(READER_BIN): $(READER_SRCS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean
