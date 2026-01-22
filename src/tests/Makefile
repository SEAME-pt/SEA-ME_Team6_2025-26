IMAGE := seame-tests
BUILD_DIR := build

HOST_ROOT := $(abspath $(CURDIR)/..)
CONTAINER_ROOT := /work
CONTAINER_TESTS := /work/tests

docker-build:
	docker build -t $(IMAGE) .

test: docker-build
	docker run --rm -t \
		-v "/Users/daviduartecf/Documents/SEAME/TESTBRANCH/src":/work \
		-w /work/tests \
		seame-tests \
		bash -lc 'rm -rf $(BUILD_DIR) && cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug && cmake --build $(BUILD_DIR) -j && ctest --test-dir $(BUILD_DIR) --output-on-failure'

coverage: docker-build
	docker run --rm -t \
		-v "$(HOST_ROOT)":$(CONTAINER_ROOT) \
		-w /work/tests \
		$(IMAGE) \
		bash -lc '\
			rm -rf $(BUILD_DIR) coverage_html coverage.info coverage_filtered.info && \
			cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug -DENABLE_CPP_TESTS=ON && \
			cmake --build $(BUILD_DIR) -j && \
			ctest --test-dir $(BUILD_DIR) --output-on-failure && \
			lcov --capture --directory $(BUILD_DIR) --output-file coverage.info && \
			lcov --remove coverage.info "/usr/*" "*/_deps/*" "*/googletest/*" "*/tests/*" "*/inc/*" "*/unit/cpp/*" "*/build/*" -o coverage_filtered.info && \
			genhtml coverage_filtered.info --output-directory coverage_html \
		'

clean:
	rm -rf $(BUILD_DIR) coverage_html coverage.info coverage_filtered.info
.PHONY: docker-build test coverage clean