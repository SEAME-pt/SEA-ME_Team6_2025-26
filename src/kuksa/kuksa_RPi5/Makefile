CXX        ?= g++
PKG_CONFIG ?= pkg-config

BIN_DIR := bin
OBJ_DIR := build
SRC_DIR := src
GEN_DIR := generated
INC_DIR := inc

PUBLISHER_BIN := $(BIN_DIR)/can_to_kuksa_publisher

GEN_ABS := $(abspath $(GEN_DIR))
INC_ABS := $(abspath $(INC_DIR))

CXXFLAGS_BASE := -std=c++17 -O2 -Wall -Wextra
CXXFLAGS_BASE += -I$(INC_ABS) -I$(GEN_ABS)

ifdef SDKTARGETSYSROOT
  CXXFLAGS_BASE += --sysroot=$(SDKTARGETSYSROOT)
  CXXFLAGS_BASE += -I$(SDKTARGETSYSROOT)/usr/include

  LDFLAGS_SYSROOT := --sysroot=$(SDKTARGETSYSROOT)
  LDFLAGS_SYSROOT += -L$(SDKTARGETSYSROOT)/usr/lib -L$(SDKTARGETSYSROOT)/usr/lib64
  LDFLAGS_SYSROOT += -Wl,-rpath-link,$(SDKTARGETSYSROOT)/usr/lib
  LDFLAGS_SYSROOT += -Wl,-rpath-link,$(SDKTARGETSYSROOT)/usr/lib64
endif

GRPC_CFLAGS := $(shell $(PKG_CONFIG) --cflags grpc++ protobuf 2>/dev/null)
GRPC_LIBS   := $(shell $(PKG_CONFIG) --libs   grpc++ protobuf 2>/dev/null)

GRPC_LIBS := $(filter-out -lutf8_validity -lutf8_range,$(GRPC_LIBS))

CXXFLAGS := $(CXXFLAGS_BASE) $(GRPC_CFLAGS)
LDFLAGS  := $(LDFLAGS_SYSROOT) $(GRPC_LIBS) -lpthread

# ---- Sources ----
GEN_SRCS := \
  $(GEN_DIR)/kuksa/val/v2/types.pb.cc \
  $(GEN_DIR)/kuksa/val/v2/types.grpc.pb.cc \
  $(GEN_DIR)/kuksa/val/v2/val.pb.cc \
  $(GEN_DIR)/kuksa/val/v2/val.grpc.pb.cc

APP_SRCS := \
  $(SRC_DIR)/main.cpp \
  $(SRC_DIR)/dispatch_frames.cpp \
  $(SRC_DIR)/kuksa_client.cpp \
  $(SRC_DIR)/is_stm_connected.cpp \
  $(SRC_DIR)/handlers/speed.cpp \
  $(SRC_DIR)/handlers/environment.cpp \
  $(SRC_DIR)/handlers/heartbeat_stm.cpp \
  $(SRC_DIR)/handlers/tof_distance.cpp \
  $(SRC_DIR)/handlers/imu_gyro.cpp \
  $(SRC_DIR)/handlers/imu_accel.cpp \
  $(SRC_DIR)/handlers/emergency_stop.cpp \
  $(SRC_DIR)/handlers/battery.cpp \
  $(SRC_DIR)/handlers/joystick.cpp

SRCS := $(APP_SRCS) $(GEN_SRCS)

# Map both .cpp and .cc to objects under build/
OBJS_CPP := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(filter %.cpp,$(SRCS)))
OBJS_CC  := $(patsubst %.cc,$(OBJ_DIR)/%.o,$(filter %.cc,$(SRCS)))
OBJS     := $(OBJS_CPP) $(OBJS_CC)

all: check-generated $(PUBLISHER_BIN)

check-generated:
	@test -f $(GEN_DIR)/kuksa/val/v2/types.pb.h
	@test -f $(GEN_DIR)/kuksa/val/v2/val.pb.h

check-env:
	@echo "=== Build env ==="
	@echo "CXX=$(CXX)"
	@echo "SDKTARGETSYSROOT=$(SDKTARGETSYSROOT)"
	@echo "CXXFLAGS=$(CXXFLAGS)"
	@echo "LDFLAGS=$(LDFLAGS)"
	@echo

$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

# Compile .cpp
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile .cc 
$(OBJ_DIR)/%.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link (objects only)
$(PUBLISHER_BIN): $(OBJS) | $(BIN_DIR)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: all clean check-env check-generated
