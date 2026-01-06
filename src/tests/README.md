# 🧪 Tests

> **Disclaimer:** This document was created with the assistance of AI technology. The AI provided information and suggestions based on the references, which were then reviewed and edited for clarity and relevance. While the AI aided in generating content, the final document reflects our thoughts and decisions.

## 📚 Index
  - [👋 Introduction](#sec-intro)
  - [🧠 Core Ideas](#sec-core-ideas)
  - [📖 Document Overview](#sec-overview)
  - [🏗️ Test Structure and Guidelines](#sec-structure)
  - [✍️ Writing Tests](#sec-writing-tests)
  - [▶️ Running Tests](#sec-running-tests)
  - [📊 Coverage Reports](#sec-coverage)
  - [🚨 Issues Log](#sec-issues)
  - [🔗 Links](#sec-links)

<a id="sec-intro"></a>
## 👋 Introduction

This folder contains all automated tests for the SEA:ME Team 6 project. Our testing suite uses a dual-language approach with C++ and Python, ensuring code quality and catching regressions early across both backend components and supporting scripts.  
**Testing Stack:**
- **C++:** GTest and Qt Test Framework.
- **Python:** Python unittest/pytest framework
- **Coverage:** lcov/genhtml for C++, coverage.py for Python.
- **Build System:** CMake with modular coverage targets.

---

<a id="sec-core-ideas"></a>
## 📖 Document Overview

Before diving into the tests, familiarize yourself with our testing approach:  
- [Software-Testing.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Software-Tests-guide.md): Covers testing fundamentals, types of tests (unit, integration, E2E) and our testing philosophy.
- [Testing-Framework-Decision.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Testing-Framework-Decision.md): Explains why we chose our current testing framework and the evaluation process.

---

<a id="sec-overview"></a>
## 🧠 Core Ideas

- To be added

---

<a id="sec-structure"></a>
## 🏗️ Test Structure and Guidelines

```bash
tests/
├── CMakeLists.txt                  # Main CMake Configuration
├── cmake/                          # CMake Modules for Coverage
│   ├── CoverageCpp.cmake           # C++ Coverage
│   ├── CoverageIndex.cmake         # Unified Coverage index generation
│   └── CoveragePython.cmake        # Python Coverage
├── unit/                           # Unit Tests
│   ├── cpp/                        # C++ unit tests
│   │   └── *_test.cpp
│   └── python/                     # Python unit tests
│       └── requirements.txt        # Python test dependencies
│       └── test_*.py
└── utils/                          # Test utilities
    └── scripts/                    # Helper scripts
        └── generate_index.sh       # Generates Unified Coverage
        └── run_python_coverage.sh  # Runs Python Coverage
```

### Naming Conventions:

- **C++ Tests:** `*_test.cpp` (e.g., `speedprovider_test.cpp`).
- **Python Tests:** `test_*.py` (e.g., `test_x.cpp`). (FALTA EXEMPLO DE DOCUMENTO DE PYTHON)
- Mirror the source structure where possible.

### Source Integration:

- C++ tests link against backend sources from `../cluster/src/backend/`.
- Tests are compiled with coverage flags (`--coverage -O0 -g`).

---

<a id="sec-writing-tests"></a>
## ✍️ Writing Tests

### Adding a C++ Test (GTest + Qt)  
1. Create a test file in `tests/unit/cpp/` following the naming convention `*_test.cpp`
2. Include required headers:
```cpp
#include <gtest/gtest.h>
#include "x.hpp"
#include <QtCore>
#include <QtTest>
```
3. Write your test using GTest macros with descriptive test suite and names:
```cpp
// Test naming format: TEST(ComponentName, BehaviorBeingTested)

// Example: Testing constructor initialization
TEST(YourComponentTest, ConstructorInit) {
    YourComponent component
    EXPECT_EQ(component.getValue(), expectedInitialValue)
}

// Example: Testing behavior within normal bounds
TEST(YourComponentTest, BehaviorWithinBounds) {
    YourComponent component

    component.setValue(25.5)
    EXPECT_EQ(component.getValue(), 25.5)
}
```

4. For Qt signal/slot testing, use `QSignalSpy`:
```cpp
// This example of signal changes depending on the logic of the component
TEST(YourComponentTest, BehaviorWithinBounds) {
    YourComponent component

    component.setValue(10.0)
    EXPECT_EQ(spy.count(), 1)   // Signal emitted once the value

    component.setValue(10.0)
    EXPECT_EQ(spy.count(), 1)   // Signal NOT emitted again
}
```

5. Common Test Scenarios to consider:  
- **Boundary Testing:** Values at minimum, maximum, just below minimum and just above max.
- **Signal Testing:** Verify that signals are emitted when they should be, and not emitted when they shouldn't.
- **State Persistent:** Test that same values don't trigger unnecessary updates.
- **Data Formatting:** Test rounding, precision, string conversion.
- **Invalid Input:** Negative values, null pointers, out-of-range values.
- **Multiple Operations:** Sequential calls, state changes over time.
- **Default Values:** Constructor initialization, reset behavior.
- **Qt Integration:** Signal/slot connections, event loop behavior.
- **Thread Safety:** Concurrent access.
- **Performance:** Response time for critical operations.

6. Update `CMakeLists.txt` to include your new test file:
```cmake
add_executable(SEA_ME_TEAM6_Tests
    ${BACKEND_SOURCES}
    unit/cpp/x_test.cpp         #Add here
)
```

7. Rebuild and run to verify your test works.

#### C++ Testing Best Practices:  
- Use descriptive test names that explain the scenario (e.g., `SpeedAboveMax`, `SpeedBelowMin`).
- Group related tests in the same test suite (first parameter of `TEST()`).
- Test the path, edge cases and error conditions.
- Use `QSignalSpy` to verify Qt signals are emitted correctly.
- Test that signals are not emitted when they shouldn't be.
- Verify string conversions with `toStdString()` for better error messages.
- Test floating-point precision and rounding behavior.

### Adding a Python Test
1. Create test file in `tests/unit/python/` following the naming convention `test_*.py`.
2. Import required modules:
```python
import pytest
from x_module import x_function, y_function, xClass
```

3. Write simple function tests for different scenarios with descriptive names:
```python
# Test basic functionality
def test_function_with_valid_input():
    assert x_function(2, 3) == 5
    assert x_function(10, 5) == 15

# Test negative numbers
def test_function_negative_input():
    assert x_function(-2, -3) == -5
```

4. Test exception handling using `pytest.raises`:
```python
def test_function_raises_error():
    with pytest.raises(ValueError):
        y_function(10, 0)
```

5. Common test scenarios to consider:
- **Basic Functionality:** Valid inputs.
- **Edge Cases:** Empty inputs, boundary values, special characters.
- **Exception Handling:** Invalid inputs that should raise errors.
- **Type Validation:** Wrong types, None values, mixed types.
- **Data Validation:** Format checking, range validation.
- **Return Values:** Correct types, expected values, None handling.
- **State Management:** Object state before/after operations.
- **Integration:** Multiple functions working together.
- **Performance:** Large datasets, repeated operations.

6. Install dependencies if needed in `requirements.txt`

7. Run your test using the Python coverage script or directly with pytest.

#### Python Testing Best Practices:
- Use descriptive test function names that explain what's beeing tested.
- Keep tests simple and focused on one behavior per test.
- Use `pytest.raises()` context manager for exception testing.
- Group related assertions when testing the same function with different inputs.
- Consider splitting complex tests into multiple smaller tests.
- Use fixtures for complex setup/teardown operations.

---

<a id="sec-running-tests"></a>
## ▶️ Running Tests

### Build and Configure:  
```bash
# Create the build directory
$ mkdir build && cd build
    # If build directory already exists, remove all of it's files
    $ cd build && rm -rf *

# Configure with CMake (enable both C++ and PYthon tests)
$ cmake .. -DENABLE_CPP_TESTS=ON -DENABLE_PYTHON_TESTS=ON

# Build the tests
$ make
```

| Command    | Behavior |
| -------- | ------- |
| `make`  | Only buils the C++ test executable (compilation step) |
| `make python_coverage` | Runs Python tests with coverage |
| `make cpp_coverage`    | Runs C++ tests with coverage |
| `make coverage`    | Runs both C++ and Python tests with coverage |


### Running C++ Tests:
```bash
# Run all C++ tests via CTest
$ ctest --output-on-failure

# Or run the test executable
$ ./SEA_ME_Team6_Tests

# Run specific test suite
$ ./SEA_ME_Team6_Tests --gtest_filter=ComponentName.*

# List all available tests
$ ./SEA_ME_Team6_Tests --gtest_list_filter
```

### Running Python Tests:
```bash
# Navigate to the Python test directory
cd ../unit/python

# Run all Python tests
pytest

# Run specific test file
pytest test_*.py

# Run specific test function
pytest test_*.py::test_x

# Run with verbose output
pytest -v

# Run with detailed output showing print statements
pytest -v -s

# Run tests matching a pattern
pytest -k "divide"
```

If you encounter any errors such as `Command 'pytest' not found`, refer to the [🚨 Issues Log](#sec-issues) for troubleshooting, specifically:
  - [Issue #1 - pytest Command not found](#issue-1)

### Generating Coverage Reports
```bash
# Generate C++ coverage
make cpp_coverage

# Generate Python coverage
make python_coverage

# Generate unified coverage report (C++ + Python)
make coverage
```

The coverage output location is: `tests/coverage/index.html`


### CMake Options
```bash
# Disable C++ tests
cmake .. -DENABLE_CPP_TESTS=OFF

# Disable Python tests
cmake .. -DENABLE_PYTHON_TESTS=OFF
```

---

<a id="sec-coverage"></a>
## 📊 Coverage Reports

### Understanding Coverage  
Our coverage system generates separate reports for C++ and Python, then creates a unified index page for easy navigation.

#### C++ Coverage (lcov/genthml):
- Captures line and branch coverage for backend C++ code.
- Excludes system header (`/usr/*`), test files, and auto generated code.
- Configured with `geninfo_unexecuted_blocks=1` for accurate reporting.

#### Python Coverage (coverage.py):
- Captures coverage for Python unit tests.
- Reports generated in HTML format.

#### Unified Index:
- Single entry point to access both coverage reports.
- Generated by `utils/scripts/generate_index.sh`.

#### Interpreting Results
- **Green/High Coverage (>80%):** Well-tested code, good to go.
- **Yellow/Medium Coverage (50-80%):** Consider adding more tests.
- **Red/Low Coverage (<50%):** Needs immediate attention.

### Coverage Best Practices
- Run coverage reports regularly during development.
- Focus on covering critical paths and edge cases.
- Don't chase 100% coverage at the expense of meaningful tests.
- Use coverage to find untested code, not as the only quality metric.

---

<a id="sec-issues"></a>
## 🚨 Issues Log

This section tracks common issues encountered during development, testing or deployment of the tests.  
Each issue includes its symptoms, probable cause and recommended fix or workaround.

<a id="issue-1"></a>

### Issue #1 -  pytest Command not found
**- Error Example:**
```bash
$ pytest

zsh: command not found: pytest
```

**- Cause:**  
`pytest` is not available in the current environment.  
On recent Ubuntu versions (Python 3.12+), the system Python environment is externally managed, which prevents installing Python packages globally using `pip`.  
As a result:
- `pip install pytest` fails or is blocked.
- `pytest` is never installed into a directory included in the system `PATH`.
- The shell cannot locate the `pytest` executable.

This commonly occurs when attempting to install testing tools system-wide instead of using a virtual environment.

**- Solution:**  
Install Python virtual environment support (if not already installed):

```bash
$ sudo apt install python3-full python3-venv
```

Create a virtual environment inside your project directory:

```bash
$ python3 -m venv .venv
```

Activate the virtual environment:

```bash
$ source .venv/bin/activate
```

Install the dependencies inside the virtual environment:
```bash
$ pip install -r requirements.txt
```

You should now be able to run:
```bash
$ pytest
```

Deactivate the virtual environment when finished:
```bash
$ deactivate
```

<!-- <a id="issue-x"></a>

### Issue #x - 
**- Error Example:**

**- Cause:** 

**- Solution:** -->

---

<a id="sec-links"></a>
## 🔗 Links

**Internal Documentation:**
- https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Software-Tests-guide.md
- https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Testing-Framework-Decision.md

**External Resources:**
- https://docs.python.org/3/library/unittest.html
- https://docs.pytest.org/en/stable/
- https://sourceforge.net/projects/ltp/
- https://doc.qt.io/qt-6/qtest-overview.html

---

> **Document Version:** 1.0  
  **Last Updated:** 6th January 2026  
  **Contributor:** souzitaaaa