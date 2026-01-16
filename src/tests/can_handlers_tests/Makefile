IMAGE := kuksa-tests
BUILD_DIR := build

TESTS_DIR := $(abspath $(CURDIR))
KUKSA_DIR  ?= $(abspath $(CURDIR)/../../kuksa)

CONTAINER_TESTS := /work/can_handlers_tests
CONTAINER_KUKSA  := /work/kuksa

.PHONY: docker-build docker-shell test clean veryclean cov-clean distclean

docker-build:
	docker build -t $(IMAGE) .

docker-shell:
	docker run --rm -it -v "$(PWD)":$(CONTAINER_WORKDIR) -w $(CONTAINER_WORKDIR) $(IMAGE) bash

test: docker-build
	docker run --rm -t \
		-v "$(TESTS_DIR):$(CONTAINER_TESTS)" \
		-v "$(KUKSA_DIR):$(CONTAINER_KUKSA)" \
		-w $(CONTAINER_TESTS) $(IMAGE) \
		bash -lc 'cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug -DKUKSA_ROOT=$(CONTAINER_KUKSA) && \
		          cmake --build $(BUILD_DIR) -j && \
		          ctest --test-dir $(BUILD_DIR) --output-on-failure'

coverage: docker-build
	docker run --rm -t \
		-v "$(TESTS_DIR):$(CONTAINER_TESTS)" \
		-v "$(KUKSA_DIR):$(CONTAINER_KUKSA)" \
		-w $(CONTAINER_TESTS) $(IMAGE) \
		bash -lc '\
			rm -rf $(BUILD_DIR) coverage_html coverage.info coverage_filtered.info && \
			cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON -DKUKSA_ROOT=$(CONTAINER_KUKSA) && \
			cmake --build $(BUILD_DIR) -j && \
			ctest --test-dir $(BUILD_DIR) --output-on-failure && \
			lcov --capture --directory $(BUILD_DIR) --output-file coverage.info && \
			lcov --remove coverage.info "/usr/*" "*/_deps/*" "*/googletest/*" -o coverage_filtered.info && \
			genhtml coverage_filtered.info --output-directory coverage_html \
		'

clean:
	rm -rf $(BUILD_DIR)

cov-clean:
	# GCC/Clang coverage artifacts
	find . -type f \( -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" \) -delete
	# lcov/gcovr html/xml/text outputs (adjust names if you use different ones)
	rm -rf coverage coverage_html coverage-report \
	       lcov.info lcov*.info \
	       gcovr.xml gcovr.html coverage.xml

distclean: clean cov-clean
	rm -rf CMakeFiles CMakeCache.txt cmake_install.cmake \
	       compile_commands.json Testing DartConfiguration.tcl \
	       CTestTestfile.cmake
