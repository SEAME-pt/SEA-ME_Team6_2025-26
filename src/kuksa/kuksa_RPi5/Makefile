CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Igenerated
CXXFLAGS += $(shell pkg-config --cflags grpc++ protobuf 2>/dev/null)

LDFLAGS := $(shell pkg-config --libs grpc++ protobuf 2>/dev/null)
LDFLAGS := $(filter-out -lutf8_validity -lutf8_range,$(LDFLAGS))
LDFLAGS += -lpthread

BIN_DIR := bin
SRC_DIR := src
GEN_DIR := generated

PUBLISHER_BIN := $(BIN_DIR)/can_to_kuksa_publisher

GEN_SRCS := \
  $(GEN_DIR)/kuksa/val/v2/types.pb.cc \
  $(GEN_DIR)/kuksa/val/v2/types.grpc.pb.cc \
  $(GEN_DIR)/kuksa/val/v2/val.pb.cc \
  $(GEN_DIR)/kuksa/val/v2/val.grpc.pb.cc

SRCS := \
  $(SRC_DIR)/can_to_kuksa_publisher.cpp \
  $(GEN_SRCS)

READER_BIN := $(BIN_DIR)/kuksa_reader

all: $(PUBLISHER_BIN) $(READER_BIN)

$(READER_BIN): $(SRC_DIR)/kuksa_reader.cpp $(GEN_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)


$(PUBLISHER_BIN): $(SRCS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean
