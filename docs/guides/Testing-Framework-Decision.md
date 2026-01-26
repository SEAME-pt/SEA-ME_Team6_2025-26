# 🧪 Testing Framework Selection

> **Disclaimer:** This document was created with the assistance of AI technology. The AI provided information and suggestions based on the references, which were then reviewed and edited for clarity and relevance. While the AI aided in generating content, the final document reflects our thoughts and decisions.


## 📚 Index
 - [👋 Introduction](#sec-intro)
 - [🧠 Core Ideas](#sec-core-ideas)
 - [🎯 Project Context Analysis](#sec-project-context)
 - [🔍 Framework Evaluation](#sec-framework-evaluation)
 - [✅ Recommended Testing Strategy](#sec-testing-strategy)
 - [🏛️ Testing Architecture](#sec-testing-architecture)
 - [⚒️ Minimal Implementation](#sec-implementation)
 - [🔗 Links](#sec-links)

 ---

<a id="sec-intro"></a>
## 👋 Introduction

This document outlines the testing framework selection process for the project. Our system combines embedded C/C++ running on ThreadX with a Qt-based user interface, requiring a comprehensive testing strategy that addresses both domains.  
The goal is to identify appropriate testing frameworks that can:
 - Validate C/C++ code for the control systems.
 - Test the Qt GUI interface for usability and functionality.
 - Integrate with our development workflow and CI/CD pipeline.
 - Support both unit and integration testing approaches.

This guide analyzes our specific context, evaluates suitable frameworks and recommends a practical testing strategy tailored to our project needs.

---

<a id="sec-core-ideas"></a>
## 🧠 Core Ideas

 - To be added

---

<a id="sec-project-context"></a>
## 🎯 Project Context Analysis

### System Architecture  
Our project consists of two primary components that require different testing approaches. The embedded system runs on ThreadX and handles real-time vehicle control, low-level hardware drivers for sensors, motors and actuatures, AI/ML algorithms for autonomous navigation, and safety-critical control loops requiring deterministic timing. This component operates under limited memory and processing resources, which affects our testing strategy choices.  
The UI is built with Qt and C++, providing a embedded display application for real-time data visualization and monitoring. The interface layer communicates with the embedded system but operates ina  less resource-constrained environment.

### Testing Requirements  
For the embedded code, we need unit testing of individual functions and modules to validate logic before integration. Hardware abstration layer (HAL) testing with mocks allow us to test without constant hardware access. Memory leak and resource usage monitoring helps identify issues that could cause system instability over time. Integration testing with actual hardware validates that our mocked interfaces accurately represent real behavior.  
The Qt interface requires widget functionality and interaction testing to ensure UI components work as designed. Signal/Slot mechanism validation confirms that Qt's core communication patterns function correctly. UI performance testing ensures the interface doesn't lag or freeze during operation. User workflow and usability testing validates that the interface meets user expectations.  
At the system level, we must test integration between embedded system and GUI to ensure proper communication. End-to-end functionality validation confirms complete workflows operate correctly. Performance under load and stress conditions reveals bottlenecks and breaking points. Safety and error handling scenarios verify that the system fails gracefully and recoveres appropriately.

<a id="sec-framework-evaluation"></a>
## 🔍 Framework Evaluation

### Embedded C/C++ Testing Frameworks

#### Google Test (GTest)  
Google Test is an industry-standard C++ testing framework with an extensive assertion library and test fixture support. It has matured over many years and is widely adopted across the software industry, making it easy to find documentation, examples and community support when encountering issues.  
The framework provides rich assetion macros that make test writing intuitive and readable. It supports death tests for validating certain code paths trigger expected failures, and handles exception testing. The framework integrates well with CI/CD tools and pairs naturally with Google Mock for creating mock objects to replace hardware dependencies.  
However, Google Test has a heavier footprint than frameworks designed specifically for embedded systems, which means it's primarily designed for host-based testing rather than running directly on embedded targets. It requires C++ compiler support, which might not be available on all embedded toolchains. Despise these limitations, its features make it execellent for unit testing algorithms and logic on development machines before deployment.

#### Unity + CMock  
Unity is a lightweight testing framework designed specifically for embedded C systems and is often paired with CMock for mocking capabilities. Its minimal memory footprint and pure C support make it suitable for resource-constrained environments where heavier frameworks would struggle.  
The framework can run directly on embedded targets, allowing for on-hardware testing when necessary. Its simple syntax makes integration straightforward, and CMock can automatically generate mocks from headers files, reducing manual setup work for testing with hardware dependencies.  
Unity has fewer features than GTest and a smaller community, which can make finding soultions to complex problems more challenging. Manual setup for complex test scenarios may requires more effort compared to feature-rich alternatives. However, for testing C code on embedded targets, Hardware abstration layer tseting, and low-level driver validation, Unity provides and excellent balance of capability and resource efficiency.

#### Catch2  
Catch2 is a modern, header-only C++ testing framework with clean syntax and minimal setup requirements. Being header-only means integrationj is as simple as including the header files in your project without managing separate library builds.  
The framework uses expressive sybtax with natural language constructs, making tests readable even for those unfamiliar with the codebase. It offers good documentation for getting started quickly. Compared to GTest, Catch2 is more lightweight while still providing substancial functionality.  
The framework only supports C++, lacking pure C compatibility, and has a smaller ecosystem than GTest. While lighter than Gtest, it may still be too heavy for direct embedded target testing in resource-constrained environments. Catch2 excels at C++ unit testing on host machines, particularly for modern C++ codebases that can leverage its clean syntax.

#### CppUTest  
CppUTest is a C/C++ unit testing framework specifically designed for embedded systems with built-in mocking support. Unlike GTest, which needs Google Mock as a separate component, CppUTest includes mocking functionality out of the box.  
The framework was designed with embedded environments in mind and can run on target hardware when necessary. It supports both C and C++ and includes memory leak detection, which is cruacial for embedded systems that must run reliably for extended periods.  
CppUTest is less popular than GTest, resulting in fewer community resources and exemples. The framework has its own syntax conventions that require a learning curve compared to more mainstream alternatives. For embedded projects requiring on-target testing with integrated mocking, CppUTest provides a specialized solution worth considering.

### Qt Testing Framework

#### Qt Test
Qt Test is the official testing framework integrated into the Qt Framework, designed specifically for testing Qt applications. This native integration eliminates compability issues and ensures that Qt-specific features are properly supported.  
The framework supports GUI testing through functions like QTest:qWait() and mouse/keyboard simulation, allowing automated validation of user interactions. Signal/slot Testing mechanismsverify Qt's core communication patterns, while benchmark support helps identify performance issues. Date-driven testing capabilities enable running the same test with multiple input sets, and the framework comes included with Qt, requiring no additional dependencies.  
Qt Test is Qt-specific and not suitable as a general-purpose testing framework. It requires Qt knowledge to use effectively and has a less extensive assertion library than general frameworks like GTest. However, for testing Qt Widgets, signals/slots, GUI interactions and Qt-specific functionality, it's the natural choice that provides the best integration with the framework.

<a id="sec-testing-strategy"></a>
## ✅ Recommended Testing Strategy

### Primary Framework Selection

For C/C++ code, we will be using GTest combined with Google Mock as the primary testing framework. This choice is based on its industry-standard status, extensive documentation and large community support that makes problem-solving easier. The rich feature set provides comprehensive unit testing capabilities, while Google Mock offers powerful mocking for hardware abstraction. The excellent CI/CD integration means we can automate test execution in our development pipeline with minimal setup.  
GTest is host-based testing approach is practical for most embedded logic, allowing developers to write and run tests quickly on their development machines without needing hardware access. We'll use this framework to unit test all algorighmic and logic components, and mock hardware interfaces to isolate code under test. The framework will run tests automatically in our CI/CD pipeline, catching regressions before they reach the main branch.  
For Qt GUI code, we will use Qt Test as the testing framework. Its native Qt integration eliminates compatibility concerns and provides access to Qt-specific testing features that external frameworks cannot replicate. Built specifically for Qt application testing, it supports GUI event simulation and signal/slot validation that are essential for validating UI behavior. Since Qt Test is already available with the Qt framework, ther's no additional setup overhead or dependency management required.  

### Complementary Approach

While Gtest and Qt Test form our primary testing strategy, there may be specific cases where Unity and CMock provide advantages, These cases include testing pure C drivers that need to run on actual hardware, validating low-level Hardware abstration layer implementations on target systems, resource-constrained scenarios where Google Test's footprint too large, and situations where on-target testing is necessary for timing validation that cannot be accurately simulated.  
Unity and CMock should be considered optional tools to complement our primary strategy rather than replace it.  

---

<a id="sec-testing-architecture"></a>
## 🏛️ Testing Architecture

Not defined yet

---


<a id="sec-implementation"></a>
## ⚒️ Minimal Implementation

Not implemented yet

---

<a id="sec-links"></a>
## 🔗 Links

- https://google.github.io/googletest/
- https://google.github.io/googletest/gmock_for_dummies.html
- https://doc.qt.io/qt-6/qtest-overview.html
- https://github.com/ThrowTheSwitch/Unity
- https://github.com/ThrowTheSwitch/CMock
- https://github.com/catchorg/Catch2
- https://cpputest.github.io/
- https://interrupt.memfault.com/blog/unit-testing-basics
- https://google.github.io/googletest/primer.html
- https://github.com/marketplace/actions/run-cmake
- 

---

> **Document Version:** 1.1  
  **Last Updated:** 15th December 2025  
  **Contributor:** souzitaaaa