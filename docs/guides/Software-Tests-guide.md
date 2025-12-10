# 🚀 Software Tests Guide

> **Disclaimer:** This document was created with the assistance of AI technology. The AI provided information and suggestions, which were then reviewed and edited for clarity and relevance. While the AI aided in generating content, the final document reflects my thoughts and decisions.


## 📚 Index
 - [👋 Introduction](#sec-intro)
 - [🧠 Core Ideas](#sec-core-ideas)
 - [🤔 Types of Tests](#sec-types-of-tests)
    - Types of Testing
        - [Manual Testing](#sec-manual-testing)
        - [Automated Testing](#sec-automated-testing)
    - Testing Approaches
        - [White Box Testing](#sec-white-box-testing)
        - [Black Box Testing](#sec-black-box-testing)
        - [Gray Box Testing](#sec-gray-box-testing)
    - Functional vs Non-Functional Testing
        - [Functional Testing](#sec-functional-testing)
        - [Non-Functional Testing](#sec-non-functional-testing)
    - Stages of Testing
        - [Unit Testing](#sec-unit-testing)
        - [Integration Testing](#sec-integration-testing)
        - [System Testing](#sec-system-testing)
        - [Performance Testing](#sec-performance-testing)
        - [Usability Testing](#sec-usability-testing)
        - [Compatibility Testing](#sec-compatibility-testing)
 - [⚒️ Tools & Best Practices](#sec-qml-elements)
 - [🔗 Links](#sec-links)

 ---

<a id="sec-intro"></a>
## 👋 Introduction

Software Testing is the process of verifying and validating whether a product or application works as intended. It helps identify errors and bugs that could affect the performance, reliability or user experience of an application.  
Software Testing can be broadly divided into two key activities in the software development process:
 - Verification: Checks whether the software is being built correctly according to specifications, In other words, it answers the question: **"Are we building the product the right way?"**.
 - Validation: Ensures that the software meets the customer's needs and requirements, Essentially, it answers the question: **"Are we building the right product?"**.

Testing is crucial for several reasons:
 - Product Quality: Testing ensures the delivery of a high-qualify product as the errors are discovered and fixed early in the development cycle.
 - Customer Satisfcation: Software testing aims to detect the errors or vulnerabilities in software early in the development phase so that the detected bugs can be fixed before the delivery of the product.
 - Cost-Effective: Testing any project on time helps to save money and time for the long term. If the bugs are caught in the early phases of software testing, it costs less to fix those errors.
 - Maintainability: Well-tested software is easier to maintain and extend.

---

<a id="sec-core-ideas"></a>
## 🧠 Core Ideas

 - **Verification and Validation:** Continuous checks to ensure the product is built correctly and meets user needs.
 - **Human Insight in Testing:** Emphasizes the importance of tester intuiton and exploration in software performance.
 - **Bug Detection Early:** Early testing identifies defects before they impact the user experience, reucing long-term costs.
 - **Testing Types Balance:** A combination of manunal and automated tests enchances effectiveness and coverage.
 - **Functional vs Non-Functional:** Distinction between testing how the software operates and attributes like performance.
 - **Testing Stages:** Different levels of testing (unit, integration, system) serve unique purposes in the development cycle.
 - **Cost of Errors:** Fixing bugs in the early phases is far less expensive than resolving issues post-release.
 - **Maintainability:** Well-tested software is easier to update and manage over time, ensuring long-term quality.
 - **User-Centric Approach:** Testing should prioritize the end-user experience to ensure usability and satisfaction.
 - **Risk Assessment:** Identifying high-risk features allows for focused testing efforts where they are most needed.

---

<a id="sec-types-of-tests"></a>
## 🤔 Types of Tests

Software Testing can be categorized into several types based on scope, purpose and methodology. Understanding these types helps developers and QA engineers choose the right approach for ensuring software quality. In this guide we will cover the following:
- **Types of Testing**
    - Manual Testing: Testers manually execute test cases without automation tools.
    - Automation Testing: Uses tools and scripts to automatically run test cases, increasing efficiency.
- **Testing Approaches**
    - White Box Testing: Focuses on the internal structure and logic of the code.
    - Black Box Testing: Evaluates the software's functionality without looking at the internal code structure.
    - Grey Box Testing: Combines white and black box testing elements, offering a more comprehensive approach.
- **Functional vs Non-Functional Testing**
    - Functional Testing: Assesses specific functions or features of the software.
    - Non-Functional Testing: Evaluates performance, usability, reliability, etc.
- **Stages of Testing:**
    - Unit Testing: Testing individual components of units.
    - Integration Testing: Testing combined application parts to ensure they work together.
    - System Testing: Testing the complete and integrated software.
    - Performance Testing: Evaluates how the software performs under various conditions, such as load, stress, scalability or stability.
    - Usability Testing: Assesses how easy and intuitive the software is for end-users.
    - Compatibility Testing: Verifies that the software works correctly accross different environments, including operating systems, browsers, devices and hardware configurations.

Combining these testing types helps teams can identify defects early, ensure system reliability and deliver high-quality software that meets both technical and user requirements.

<div style="text-align: center;">
    <img src="https://media.geeksforgeeks.org/wp-content/uploads/20230808151753/Software-Testing-Types-768.png" alt="Software Testing Types">
</div>


<a id="sec-manual-testing"></a>
### Manual Testing

Manual Testing is the process of testing software by manually executing test cases without the use of automation tools. It envoles a tester actively using the application to identify bugs, verify functionality and ensure the software meets user requirements.  
Manual testing is essential because it allows human insight into the software's behavior, which is especially valuable for exploring new applications before automating tests. It requires no knowledge of testing tools, but it can be time-consuming and prone to human error.  
There are three key types of Manual Testing: Black Box, White Box and Gray Box, but they will be covered up latter on this guide.  

**Steps in Manual Testing**  
 - Requirement Analysis: Thoroughly review functional and non-functional specifications to understand what the software is supossed to do.
 - Test Planning: Define the scope, objectives, resources and schedule of testing activities. Identify the types of tests needed and assign responsibilities.
 - Test Case Design: Develop detailed test cases with inputs, expected results and execution steps. Ensure coverage of all functional scenarios, edge cases and potential failure points.
 - Test Execution: Perform the test cases manually, documenting actual results, defects and observations. Report deviations from expected behavior.
 - Defect Reporting & Tracking: Log defects in a tracking system, communicate issues to developers, and follow up until resolution.
 - Retesting & Regression Testing: After bugs are fixed, retest the specific scenarios and conduct regression testing to ensure changes have not affected other parts of the application.

**Advantages of Manual Testing**  
 - Human Insight: Captures usability issues, interface inconsistencies and user experience problems that automated tests might miss.
 - Flexible & Exploratory: Allows testers to explore the software freely, adapt to new findings and test unusual scenarios.
 - Minimal Setup: Does not require automation tools, making it suitable for small projects or early-stage testing.
 - Quick for Small Changes: Efficient for validating minor modifications or updates without the overhead of automation scripts.

 **Limitations of Manual Testing**  
 - Time-Consuming: Manual execution of large test suites can take significant time and effort.
 - Prone to Human Error: Test outcomes may vary between testers, and repetitive tasks increase the risk of oversight.
 - Limited Scalability: Inefficient for large-scale applications or frequent regression cycles.
 - Difficulty in Repetition: Manual tests are harder to repeat consistently, which can CI/CD processes.

<a id="sec-automated-testing"></a>
### Automated Testing

Automation Testing uses specialized software or scripts to perform testing tasks that would otherwise be done manually. It is particularly valuable for repetitive, time-consuming or high-volume test cases and integrates seamlessly with modern development practices such as Agile and DevOps.  
Automation testing improves speed, consistency and accuracy, while allowing teams to focus on exploratory and high-level testing activities that require human judgment.

**Key Features of Automation Testing**  
 - Scripted Execution: Test cases are written as scripts and can be run automatically without manual intervention.
 - Reusability: Test scripts can be reused across multiple test cycles or projects, saving time and effort.
 - Continuous Integration & Delivery: Automation supports frequent code integration and rapid deployment by validating changes continuously.
 - Detailed Reporting: Automation tools can log results, compare actual vs expected outcomes and generate reports automatically.

**Advantages of Automation Testing**  
 - Higher Test Coverage: More test cases can be executed compared to manual testing, improving overall coverage.
 - Faster Execution: Automation dramatically reduces testing time, enabling quicker release cycles.
 - Consistency & Accuracy: Eliminates human error and ensures tests are performed the same way every time.
 - Cost-Efficient in the Long Run: Initial investment in automation pays off through reduced manual effort and faster feedback loops.
 - Frequent Testing: Supports continuous testing during development, ensuring code changes do not break existing functionality.

**Limitations**  
 - Initial Setup Costs: Requires investment in tools, frameworks and script development.
 - Maintenance Overhead: Test scripts need regular updates when software changes.
 - Not Suitable for All Tests: Usability tests still require human judgment.
 - Complex for Dynamic Applications: Highly dynamic interfaces may require advanced scripting and frequent adjustments.

Automation Testing complements Manual Testing by handling repetitive, high-volume tasks efficiently, while Manual Testing remains essential for tasks that require human insight, creativity and judgment.

<a id="sec-white-box-testing"></a>
### White Box Testing

White Box Testing (also known as structural or code-based testing) focuses on the internal logic, structure and workings of a software application. Testers have access to the source code and design test cases that verify the correctness of the code, control flow, and data handling.  
This type of testing ensures that the software behaves as intended at the code level and is especially useful for detecting logical errors, optimizing performance and verifying all possible paths through the application.  

**Key Focus Areas**
 - Code Logic and Flow: Ensures that control structures (if-else, loops, switches) and functions operate correctly.
 - Code Coverage: Validates that all lines, branches and paths of code are exercised by tests, identifying untested or "dead" code.
 - Data Flow and Variables: Checks proper initialization, updates and usage of variables to prevent errors.
 - Internal Functions and Methods: Confirms that functions and methods perform their intended tasks accurately.
 - Boundary Conditions: Tests edge cases such as minimum/maximum inputs or loop iterations.
 - Error Handling and Exception Management: Verifies that exceptions and invalid inputs are managed correctly.

**Types of White Box Testing**
 - Path Testing: Ensures all possible execution paths are tested.
 - Loop Testing: Validates correctness and efficency of loops.
 - Unit Testing: Tests individual functions or modules.
 - Mutation Testing: Evaluates the quality of existing test cases by introducing small changes to code.
 - Integration Testing: Checks how different components work together.
 - Penetration Testing: Tests for vulnerabilities, such as code injections, in a controlled "attack" scenario.

**Process of White Box Testing**
 - Input Gathering: Collect requirements, functional specifications, design documents and source code.
 - Planning and Prioritizing: Identify risk areas, define test cases, and prioritize tests based on potential impact.
 - Test Execution: Run the designed test cases, verify outputs and document defects.
 - Analysis & Reporting: Analyze results, report bugs, and ensure coverage of all critical paths.

<div style="text-align: center;">
    <img src="https://media.geeksforgeeks.org/wp-content/uploads/20250219162443427855/Process-of-White-Box-Testing.webp" alt="Software Testing Types">
</div>

White Box Testing is crucial for improving code quality, uncovering hidden errors, and ensuring that internal software logic functions as intended.

<a id="sec-black-box-testing"></a>
### Black Box Testing

Black Box Testing is a software testing method in which the testers evaluates the functionality of the application without any knowledge of its internal code, structure, or implementation details. The focus is entirely on validating that the software meets its specified requirements and behaves correctly from the user's perspective.

**Key Focus Areas**  
 - Functional Testing: Tests the software against functional requirements. Testers provide inputs and compare actual outputs with expected results to ensure features work correctly.
 - Regression Testing: Ensures that new code changes do not break existing functionality. It validates that the software remains stable after updates or enhancements.
 - Non-Functional Testing: Assesses attributes such as perfomance, usability, reliability and scability. This ensures the software meets quality standards beyond basic functionality.

**Advantages of Black Box Testing**  
 - Does not require programming knowledge, making it accessible to non-developer testers.
 - Tests software from the user's perspective, highlighting real-world usage issues.
 - Test cases are easily reproducible and help identify ambiguities in requirements.
 - Effective for larger systems where internal knowledge may be impractical.

**Limitations of Black Box Testing**  
 - May involve repeated testing due to overlapping or redundant test cases.
 - Difficult to design tests without clear functional specifications.
 - Cannot detect internal code errors or control structure issues.
 - Handling large input sets can be time-consuming and exhaustive.
 - Test failures may not clearly indicate the source of the problem.

Black Box Testing complements white box testing by focusing on the software's behavior rather than its internal implementation. Together with the White Box Testing, they provide a comprehensive approach to verifying both code quality and functional correctness.

<a id="sec-gray-box-testing"></a>
### Gray Box Testing

Gray Box Testing is a hybrid software testing approach that combines elements of both Black Box and White Box Testing. Testers have partial knowledge of the internal workings of the system, but the main focus remains on validating inputs, outputs and user-facing functionality. This approach leverages the strengths of both testing types, allowing for mere comprehensive coverage.

**Objectives**  
 - Combine advantages of Black Box and White Box testing.
 - Improve product quality by addressing both functional and structural concerns.
 - Reduce testing overhead while ensuring critical areas are covered.
 - Enable developers to efficiently fix defects discovered during testing.
 - Test from the user's perspective while considering internal system behavior.

**Techniques**  
 - Matrix Testing: Examines business and technical risks of variables defined by developers.
 - Pattern Testing: Analyzes previous defects to proactively prevent similar issues.
 - Orthogonal Array Testing: Uses permutations and combinations of test data to achieve maximum coverage with minimal test cases.
 - Regression Testing: Ensures that changes or new functionality do not break existing behavior.
 - State Transition Testing: Validates correct handling of various system states.
 - Decision Table Testing: Organizes complex business rulse to generate comprehensive test cases.
 - API Testing: Tests exposed interfaces to ensure correct input handling and functionality.
 - Data Flow Testing: Checks the flow of data through the system to detect processing issues.

**Process of Gray Box Testing**  
 - Identify Inputs: Determine which inputs are critical for testing based on partial system knowledge.
 - Predict Outputs: Define expected results to validate system behavior.
 - Select Key Testing Paths: Focus on essential features and their internal and external interactions.
 - Spot Sub-Functions: Identify smaller components that may require targeted testing.
 - Define Inputs for Sub-Functions: Choose precise inputs to evaluate these components.
 - Predict Expected Outputs for Sub-Functions: Determine expected results for smaller features.
 - Execute Sub-Function Test Cases: Apply real-world scenarios and inputs.
 - Verify Results: Compare actual outcomes with expected results to confirm correctness.

Gray Box Testing provides a balanced approach, offering the insight of White Box Testing while maintaining the usability-focused perspective of Black Box Testing. It is especially effective for API testing, complex workflows and systems where partial knowledge can enhance testing efficiency.

<a id="sec-functional-testing"></a>
### Functional Testing

Functional testing is a type of software testing that verifies whether each function of an application works according to its requirements and specifications. Unlike White Box Testing, functional testing does not consider the internal code structure - it focuses solely on the behavior of the software from the user's perspective.  
This type of testing ensures that the system performs all expected tasks correctly, providing a reliable and smooth user experience. Functional Testing can be performed manually or through automation, depending on project needs.

**Key Focus Areas**  
 - Basic Usability: Ensures users can navigate the application easily and intuitively.
 - Main Functions: Validates that core features work as intended.
 - Accessibility: Checks that the application is usable by all users, including those with disabilities, and that accessibility features are functioning properly.
 - Error Handling: Confirms that appropriate error messages are displayed and handled correctly when system failures occur.

**Functional Testing Process**  
 - Identify Test Inputs: Determine which features or functions need testing, including normal scenarios and potential error conditions.
 - Compute Expected Outcomes: Define what the correct output or system responde should be for each input based on requirements.
 - Execute Test Cases: Run the tests manually or using automation tools to apply the inputs and capture outputs.
 - Compare Actual vs Expected Output: Verify that the software behaves as expected, any discrepancies are reported as defects.

**Common Functional Testing Techniques**  
 - Unit Testing: Tests indivual components or modules for correctness.
 - Integration Testing: Evaluates interactions between combined units to detect interface issues.
 - System Testing: Tests the complete integrated system for compliance with requirements.
 - User Acceptance Testing: Validates that the software meets business requirements and user expectations in a real-word environment.
 - Regression Testing: Ensures that recent code changes do not break existing functionality.
 - Smoke Testing: Checks basic functionality to confirm that the most critical features work.

Functional testing is critical for confirming that software meets its functional requirements and delivers a relaible, user-friendly experience.

<a id="sec-non-functional-testing"></a>
### Non-Functional Testing

Non-Functional Testing focuses on evaluating the quality attributes of a software system rather than its specific behaviors or features. While functional testing checks what the system does, non-functional testing verifies how well it performs under various conditions. These tests ensure the system is usable, efficient, secure, scalable and reliable - qualities that are essential for delivering a robust, production-ready application.  
This type of testing plays a crucial role in assessing performance bottlenecks, system behavior under stress, compliance with standards, and overall user satisfaction.

**Key Focus Areas**  
 - Quality-Oriented: Targets attrivutes such as speed, reliability and usability.
 - Quantitative and Measurable: Often involves metrics like response time, throughput, memory usage or error rates.
 - Environmental-Dependent: Usually executed under simulated or real operational conditions.
 - Cross-Cutting: Applies to the entire application rather than specific functionalities.

**Objectives of Non-Functional Testing**  
 - Validate that the system meets performance benchmarks such as response time, scalability and resource utilization.
 - Ensure stability and reliability during normal and peak usage.
 - Assess user experience factos like visual accessibility, ease of navigation, and overall usability.
 - Confirm compliance with industry standards regulations and architectural requirements.
 - Reduce system risks by exposing potential operational weaknesses early in the development cycle.

**Types of Non-Functional Testing**  
 - Performance Testing: Evaluates how the system behaves under various workloads to ensure smooth and responsive operation.
    - Load Testing: Checks system performance under expected user loads.
    - Stress Testing: Pushes the system beyond normal limits to identify breaking points.
    - Volume Testing: Tests the system's ability to handle large volumes of data.
    - Spike Testing: Observes how the system reacts to sudden surges in traffic.
 - Usability Testing: Assess how easy and intuitive the software is for end-users. Focus areas include interface design, clarify of navigation, error messaging and accessibility.
 - Compatibility Testing: Validates that the application operates correctly accross different environments.

**Advantages of Non-Functional Testing**  
 - Improves user satisfaction through enhanced performance and usability.
 - Boosts system reliability under real-world conditions.
 - Identifies scalability limitations before deployment.
 - Ensures security limitations before deployment.
 - Helps meet regulatory and compliance standards.
 - Reduces long-term maintenance costs.

**Limitations of Non-Functional Testing**  
 - Requires specialized tools and environments.
 - Can be expensive due to infrastructure needs for performance or stress testing.
 - Results may vary depending on test environment complexity.
 - Often needs skilled testers with expertise in performance engineering and system architecture.

<a id="sec-unit-testing"></a>
### Unit Testing

Unit Testing is the process of testing individual components, modules or units of source code to verify that each function works correctly in isolation. A "unit" is typically the smallest testable part of an application - such as a function, method or class.  
The primary goal of unit testing is to ensure that every building block of the system works as intended before integrating it with other components.  
Unit testing is usually done by developers during the coding phase, making it an essential part of the Shift-Left Testing approach, which promotes defects as early as possible.  

<div style="text-align: center;">
    <img src="https://media.geeksforgeeks.org/wp-content/uploads/20241022151338496651/unit-test.webp" alt="Software Testing Types">
</div>

**Objectives of Unit Testing**
 - Verify that individual units behave as expected under different input conditions.
 - Detect defects early in the development cycle, reducing the cost of bug fixes.
 - Improve code quality and robustness by enforcing modular design.
 - Facilitate easier code refactoring and future enhancements.
 - Ensure that changes in the codebase do not break existing functionality

**Characteristics of Unit Testing**
 - Isolated: Tests focus solely on the unit being tested.
 - Automated: Typically executed through testing frameworks.
 - Repeatable: Produces deterministic results regardless of external factors.
 - Fast: Designed to run quickly and ofter, usually integrated into CI pipelines.
 - Code-Level Testing: Requires understanding of internal logic and structure.

**Advantages of Unite Testing**
 - Catches bugs early in development.
 - Reduces the risk of unexpected issues as the codebase grows.
 - Enables safer code refactoring and long-term maintainability.
 - Improves system design by encouraging modularity and abstraction.
 - Reduces overall debugging time and cost.

**Limitations of Unit Testing**
 - Cannot detect integration or system-level issues.
 - Time-consuming initially due to the need to write extensive test cases.
 - Highly depdendent on developer skill and discipline.

<a id="sec-integration-testing"></a>
### Integration Testing

Integration Testing is a software testing phase in which individual units or components are combined and tested as a group. The purpose is to verify the interactions between integrated modules and ensure that they work together correctly.  
While Unit Testing focuses on each component in isolation, Integration Testing evaluates how these components collaborate.  
This testing is typically performed after Unit Testing and before System Testing, forming a critical checkpoint in the software development lifecycle.

**Objectives of Integration Testing**
 - Verify that integrated modules interact as expected.
 - Detect interface-realted issues such as incorrect data formats, API mismatches, or communication errors.
 - Ensure that functional and non-functional behaviors remain consistent when units are combined.
 - Validate end-to-end flows across interconnected components.
 - Identify defects early before reaching the system testing phase.

**Why Integration Testing is Important**
 - Unit-tested components may still fail when interacting with others.
 - API changes, database interactions or service calls can break functionality.
 - Helps uncover defects related to data exchange, control flow or timing.
 - Reduces overall debugging cost and improves system reliability.

**Levels of Integration**  
Integration can be done at various depths, depending on the architecture:
 - Component Integration: Focuses on interactions between components within the same subsystem.
 - System Integration: Ensures correct communication between different systems or external interfaces.
 - Interface Integration: Targets API boundaries, data contracts, and messaging formats.

**Advantage of Integration Testing**
 - Detects interface and communication problems early.
 - Ensures that combined modules behave correctly as a system.
 - Improves overall system reliability and performance.
 - Reduces risk os cascading failures in later testing stages.

**Limitations of Integration Testing**
 - Requires more setup and environments compared to unit testing.
 - Sometimes depdends on incomplete or unstable modules.
 - Debugging integration defects may be harder than unit-level issues.
 - Stubs or drivers may not perfectly represent missing components.

<a id="sec-system-testing"></a>
### System Testing

System Testing is a high-level software testing phase where the complete, fully integrated application is evaluated as a whole. At this stage, all modules, subsystems, and components are combined into a single system and tested against the specified requirements.  
The primary goal of system testing is to verify that the entire software product behaves correctly, meets both functional and non-function requirements and provides a consistent a reliable user-experience under real-world conditions.  
System testing is typically performed by specialized QA engineers in an environment that closely mimics production.

**Objectives of System Testing**
 - Validate the complete and integrated system for correctness and quality.
 - Ensure the software meets business, functional and technical specifications.
 - Evaluate end-to-end worklows, including user interactions, system logic and backend processes.
 - Verify both functional behavior and non-functional attributes.
 - Identify defects that emerge only when all components operate together.

**What System Testing Focuses On**
 - Overall functionality: Does the system work as expected for the end user?
 - User flows and scenarios: Are complete processes working smoothly?
 - System behaviour under constraints: How does the system handle load, stress or limited resources?
 - Error hanlding and recovery: Does the system fail gracefully and recover properly?
 - Compatibility: Does the system work on multiple platforms, browsers, or devices?

**Advantages of System Testing**
 - Ensures the entire system behaves correctly, not just individual components.
 - Detects defects related to real-world workflows and interactions.
 - Validates both functional and non-functional requirements.
 - Helps ensure readiness for User Acceptance Testing and deployment.
 - Reduces the risk of critical issues appearing in production.

**Limitations of System Testing**
 - Requires significant setup and can be resource-intensive.
 - Bugs found here may be harder to trace due to system complexity.
 - Testing cycles can be longer compared to earlier stages.
 - Effectiveness depends heavily on the accuracy of requirements.

<a id="sec-performance-testing"></a>
### Performance Testing

Performance Testing ir a non-functional testing technique used to evaluate how a system behaves under various levels of load, stress and usage. Its primary objective is to ensure that the software performs reliably, efficiently and consistently when subjected to a real-world or extreme operational conditions.  
This type of testing identifies performance bottlenecks, scalability limitations, resource usage issues, and stability problems. Performance Testing is essential for applications that must support high traffic, large data volumes, or mission-critical operations.

**Objectives of Performance Testing**
 - Determine whether the system meets performance requirements for speed, scalability and stability.
 - Identify system bottlenecks such as slow database queries, memory leaks or inefficient code paths.
 - Valiate how the system handles expected, peak and extreme loads.
 - Ensure the application remains reliable under stress or prolonged usage.
 - Provide performance benchmakrs for future development and optimization.

**Key Performance Metrics**
 - Response Time: Time taken for the system to respond to a request.
 - Throughput: Number of transactions processes per second.
 - Scalability: Ability to handle increasing workloads by adding resources.
 - Latency: Delay between user request and server response.
 - Resource Utilization: CPU, memory, network, disk usage under load.
 - Concurrency: Number of simultaneous active users the system can support.
 - Error Rate: Frequency of failures when the system is under load or stress.

**Advantages of Performance Testing**
 - Ensures smooth and responsive user experience under all conditions.
 - Detects performance issues before deployment, preventing costly failures.
 - Improves system scalability, reliability and stability
 - Builds confidence for high-traffic events, releases or scaling plans.

**Limitations of Performance Testing**
 - Requires a realistic environment, which can be costly or complex to set up.
 - Performance tools may require specialized skills to use effectively.
 - Test results may vary if the environment differs from production.
 - Time-consuming when analysing complex system interactions.

<a id="sec-usability-testing"></a>
### Usability Testing

Usability Testing is a non-functional testing technique that evaluates how easy, intuitive and user-friendly a software application is. It focuses on the end-user experience, assessing whether users can interact with the system efficiently and effectively while achieving their goals with minimal frustation or errors.  
The primary goal of usability is to ensure that software is not only functional but also accessible, intuitive and satisfying to use.

**Objectives of Usability Testing**
 - Assess the ease of learning and intuitiveness of the interface.
 - Evaluate efficienct of completing tasks and workflows.
 - Identify user errors or areas of confusion in the interface.
 - Ensure the software meets user expectations and eaccessibility standards.
 - Provide actionable insights to improve user experience (UX) and satisfaction.

**Key Focus Areas**
 - Interface Design: Layout, color schemes, typography and visual hierarchy.
 - Navigation: Ease of moving through screens, menus and workflows.
 - Error Handling: Clarity and helpfulness of error messages.
 - Accessibility: Compliance with accessibility standards, support for assistive technologies.
 - Task Efficiency: How quickly and accurately users can complete tasks.
 - User Satisfaction: Overall perception of the system's usability and friendliness.

**Advantages of Usability Testing**
 - Improves user satisfaction and engagement.
 - Identifies real-world issues that might not be detect in functional testing.
 - Enhances accessibility, making software usable by a broader audience.
 - Reduces training and support costs by improving intuitiveness.
 - Encourages user-centered design, aligning software with actual user needs.

**Limitations of Usability Testing**
 - Requires access to real users, which can be costly or time-consuming.
 - Results may vary depending on participant experience or biases.
 - May not catch all technical issues or backend defects.
 - Requires careful planning and scenario design to yield meaningful insights.

<a id="sec-compatibility-testing"></a>
### Compatibility Testing

Compatibility Testing is a type of non-functional testing that ensures a software application operates correctly accross different environments, platforms, devices, browsers and network conditions. Its purpose is to guarantee consistent behavior, functionality and performance for all users, regardless of their system configurations.  
This testing is essential in today's diverse technology landscape, where users access applications on multiple devices, operating systems and browers.

**Objectives of Compatibility Testing**
 - Verify the software functions correctly across various operating systems (Windows, macOS, Linux, etc.).
 - Ensure consistent behavior on different browsers (Chrome, Firefox, Safari, Edge).
 - Validate performance on multiple devices (desktop, mobile, tablet).
 - Test under various network conditions (low bandwidth, high latency, intermittent connectivity).
 - Identify and resolve environment-specific bugs or inconsistencies.

**Key Focus Areas**
 - Operating System Compatibility: Testing across different OS versions and configurations.
 - Browser Compatibility: Ensuring web applications render and behave correctly on multiple browsers.
 - Device Compatibility: Testing on varying screen sizes, resolutions, and hardware capabilities.
 - Network Compatibility: Assessing performance and functionality under different network conditions.
 - Software Environment: Checking interactions with third-party applications, plugins, or dependencies.

**Advantages of Compatibility Testing**
 - Ensures a consistent user experience across all platforms.
 - Reduces risk of environment-specific failures after deployment.
 - Increases customer satisfaction by supporting diverse user setups.
 - Helps identify platform or configuration limitations early.
 - Supports broader adoption of software by targeting multiple devices and browsers.

**Limitations of Compatibility Testing**
 - Requires access to a wide range of devices, operating systems, and browsers, which can be costly.
 - Time-consuming due to the number of configurations to test.
 - May require emulators or simulators that do not perfectly replicate real environments.
 - Complex test planning and execution for applications with frequent updates or dependencies.

---

<a id="sec-qml-elements"></a>
## ⚒️ Tools & Best Practices



---

<a id="sec-links"></a>
## 🔗 Links

- https://www.geeksforgeeks.org/software-testing/software-testing-basics/
- https://en.wikipedia.org/wiki/Software_testing
- https://www.ibm.com/think/topics/software-testing
- https://www.globalapptesting.com/blog/software-testing#software-testing
- https://www.geeksforgeeks.org/software-testing/software-testing-manual-testing/
- https://www.geeksforgeeks.org/software-testing/automation-testing-software-testing/
- https://www.geeksforgeeks.org/software-testing/software-engineering-white-box-testing/
- https://www.geeksforgeeks.org/software-testing/software-engineering-black-box-testing/
- https://www.geeksforgeeks.org/software-testing/gray-box-testing-software-testing/
- https://www.geeksforgeeks.org/software-testing/software-testing-functional-testing/
- https://www.geeksforgeeks.org/software-testing/software-testing-non-functional-testing/
- https://www.geeksforgeeks.org/software-testing/unit-testing-software-testing/
- https://www.geeksforgeeks.org/software-testing/software-engineering-integration-testing/
- https://www.geeksforgeeks.org/software-testing/system-testing/
- https://www.geeksforgeeks.org/software-testing/performance-testing-software-testing/
- https://www.geeksforgeeks.org/software-testing/usability-testing/
- https://www.geeksforgeeks.org/software-engineering/compatibility-testing-in-software-engineering/
