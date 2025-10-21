# **GenAI Pair Programming Guidelines**

### **Purpose**

This document defines the techniques, best practices, and integration strategies for effective collaboration with Generative AI tools (e.g., ChatGPT, GitHub Copilot, Codeium).  
It ensures that AI-assisted development aligns with the **Trustable Software Framework (TSF)** principles of traceability, quality, and accountability.

---

## **1. Techniques for Effective Pair Programming with Generative AI**

| **Technique** | **Description** | **Example Application** |
|----------------|-----------------|---------------------------|
| **Turn-taking collaboration** | Alternate between writing code manually and prompting the AI for suggestions, improvements, or documentation. | The developer implements a class structure; the AI suggests complementary test cases. |
| **Prompt refinement** | Provide detailed and contextual prompts for more accurate results. | “Write a C++ function to control a servo motor on a Raspberry Pi 5 using WiringPi” instead of “write servo code.” |
| **Explain-first approach** | Explain the project’s objective and environment before requesting code. | “We’re developing a Qt GUI for the PiRacer’s screen that displays static elements only.” |
| **Collaborative debugging** | Use the AI to interpret compiler errors, logs, or stack traces. | “Here’s the CMake error when building the Qt app — what might be missing?” |
| **Code walkthroughs** | Request detailed explanations of generated code to strengthen understanding. | “Explain how this Qt signal-slot connection works.” |

---

## **2. Best Practices for Reviewing AI-Generated Code**

All AI-generated content must undergo **human validation** before being committed to the repository.

| **Best Practice** | **Objective** | **Application** |
|--------------------|----------------|-----------------|
| **Human validation first** | Ensure that AI output is logical, correct, and contextually appropriate. | Review all AI code manually before merging. |
| **Security & privacy check** | Prevent leaks of sensitive data or insecure API usage. | Review for exposed tokens, hardcoded credentials, or unsafe URLs. |
| **Style consistency** | Maintain consistent coding style and formatting. | Use automated formatters (e.g., `clang-format`, `black`) to standardize. |
| **Performance and safety review** | Validate that AI-generated code does not introduce performance or safety risks. | Check loops, memory usage, and thread safety. |
| **Source attribution** | Ensure compliance with open-source licensing. | Verify that the AI did not reproduce GPL or proprietary code snippets. |
| **Traceability** | Document the use of AI in commits and design documentation. | Mention AI usage in commit messages or PR descriptions (e.g., “Code generated with AI assistance, reviewed by human developer”). |

---

## **3. Integration Strategies for Seamless AI Assistance**

| **Strategy** | **Goal** | **Implementation** |
|---------------|----------|--------------------|
| **IDE integration** | Embed AI tools directly in the development environment. | Use extensions such as *GitHub Copilot* or *ChatGPT for VSCode*. |
| **Prompt templates** | Standardize prompts for recurring tasks. | “Generate unit tests for function X using GoogleTest.” |
| **Review loop integration** | Incorporate AI-assisted review in Pull Request workflow. | Use AI suggestions before human code review. |
| **Documentation generation** | Automate comment and documentation creation. | “Write Doxygen comments for this C++ class.” |
| **AI-assisted test generation** | Automatically generate test cases based on the implementation. | “Generate Python tests for the `display_controller` module.” |
| **Knowledge transfer** | Use AI to explain new frameworks or libraries. | “Explain the difference between `QMainWindow` and `QWidget` in Qt.” |
| **Ethical usage policy** | Define clear rules for when and how AI can be used. | Require peer review for all AI-generated contributions. |

---

## **4. Ethical and Educational Considerations**

- AI is a **support tool**, not a substitute for critical thinking.  
- The developer retains **full responsibility** for code correctness, safety, and maintainability.  
- AI-generated code must **not** contain or replicate copyrighted material.  
- All team members should maintain transparency about AI usage.  
- The objective is to **learn and improve human skill**, not to offload reasoning to AI.

---

## **5. Expected Outcomes**

By following these guidelines, the development team will:

- Improve productivity through structured AI collaboration.  
- Maintain software integrity and compliance with TSF principles.  
- Enhance learning and understanding of both AI-assisted and manual development.  
- Ensure reproducibility and accountability in all AI-supported work.

---

## **6. References**

- *ISO/IEC 5338:2022* — Guidelines for the Use of AI in Software Development  
- *Trustable Software Framework (TSF)* — Documentation and Traceability Principles  
- *GitHub Copilot Guidelines* — Responsible AI Usage Policy  
- *ChatGPT Code Interpreter Best Practices*, OpenAI (2024)

---

**Document version:** 1.0  
**Last updated:** *October 2025*  
**Author(s):** *PiRacer Warm-Up Team*  
**Repository:** [GitHub Repository Link]
