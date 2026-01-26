#!/bin/bash
set -e

PYTHON_TEST_DIR="$1"
PYTHON_COVERAGE_DIR="$2"

echo "Running Python tests with coverage..."
cd "$PYTHON_TEST_DIR"

# Verify .venv
if [ ! -d ".venv" ]; then
    echo "❌ ERROR: Virtual environment not found at $PYTHON_TEST_DIR/.venv"
    echo "   Create it with: python3 -m venv .venv"
    exit 1
fi

source .venv/bin/activate

# Verify if coverage  is installed
if ! command -v coverage &> /dev/null; then
    echo "❌ ERROR: 'coverage' not found in virtual environment"
    echo "   Install it with: pip install pytest coverage"
    exit 1
fi

# Verify if pytest  is installed
if ! command -v pytest &> /dev/null; then
    echo "❌ ERROR: 'pytest' not found in virtual environment"
    echo "   Install it with: pip install pytest coverage"
    exit 1
fi

echo "✓ Running tests..."
coverage erase
coverage run -m pytest -v
coverage html -d "$PYTHON_COVERAGE_DIR"

echo "✓ Python coverage generated at $PYTHON_COVERAGE_DIR"
deactivate