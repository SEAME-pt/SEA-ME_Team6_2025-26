# =========================
# kuksa_reader_subscriber
# =========================

NAME := kuksa_reader_subscriber

CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
INCLUDES := -Igenerated -Isrc

# Source files
SRC := \
	src/kuksa_reader_subscriber.cpp \
	generated/kuksa/val/v2/val.pb.cc \
	generated/kuksa/val/v2/val.grpc.pb.cc \
	generated/kuksa/val/v2/types.pb.cc

# gRPC / Protobuf flags (static linking as requested)
PKG_CONFIG := pkg-config
#LIBS := $(shell $(PKG_CONFIG) --cflags --libs --static grpc++ protobuf)
RAW_LIBS := $(shell $(PKG_CONFIG) --cflags --libs grpc++ protobuf)
LIBS := $(subst -lutf8_validity,,$(subst -lutf8_range,-lutf8_range_lib,$(RAW_LIBS)))

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $(NAME) $(LIBS) -pthread

clean:
	@echo "Nothing to clean (no object files)"

fclean:
	rm -f $(NAME)

re: fclean all
