IMAGE := kuksa-tests
HOST_MOUNT := /Users/daviduartecf/Documents/SEAME/SEAME-KUKSA/src/kuksa
CONTAINER_WORKDIR := /work/kuksa_tests
BUILD_DIR := build

.PHONY: docker-build docker-shell test clean veryclean cov-clean distclean

docker-build:
	docker build -t $(IMAGE) .

docker-shell:
	docker run --rm -it -v "$(PWD)":$(CONTAINER_WORKDIR) -w $(CONTAINER_WORKDIR) $(IMAGE) bash

test: docker-build
	docker run --rm -t \
		-v "$(HOST_MOUNT)":/work \
		-w $(CONTAINER_WORKDIR) $(IMAGE) \
		bash -lc 'cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build -j && ctest --test-dir build --output-on-failure'

coverage: docker-build
	docker run --rm -t \
		-v "$(HOST_MOUNT)":/work \
		-w $(CONTAINER_WORKDIR) $(IMAGE) \
		bash -lc '\
			rm -rf build coverage_html coverage.info coverage_filtered.info && \
			cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON && \
			cmake --build build -j && \
			ctest --test-dir build --output-on-failure && \
			find build -name "*.gcda" -print && \
			lcov --capture --directory build --output-file coverage.info && \
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
