IMAGE := seame-tests
BUILD_DIR := build
REPO_ROOT := $(shell git rev-parse --show-toplevel)

HOST_ROOT := $(abspath $(CURDIR)/..)
CONTAINER_ROOT := /work
CONTAINER_TESTS := /work/tests

docker-build:
	docker build -t $(IMAGE) -f ./Dockerfile .

test: 
	@./scripts/run_tests_docker.sh

coverage:
	@./scripts/coverage.sh

clean:
	rm -rf $(BUILD_DIR) coverage_html coverage.info coverage_filtered.info
.PHONY: docker-build test coverage clean