# 🐋 Docker Setup Environments

> **Disclaimer:** This document was created with the assistance of AI technology. The AI provided information and suggestions based on the references, which were then reviewed and edited for clarity and relevance. While the AI aided in generating content, the final document reflects our thoughts and decisions.

## 📚 Index
  - [👋 Introduction](#sec-intro)
  - [🧠 Core Ideas](#sec-core-ideas)
  - [🏗️ Architecture Overview](#sec-architecture)
  - [🔧 Docker Components](#sec-docker-components)
  - [🛠️ Development Workflow](#sec-workflow)
  - [📦 Building & Deployment](#sec-deployment)
  - [🔄 CI/CD Integration](#sec-cicd)
  - [🚨 Issues Log](#sec-issues)
  - [🔗 Links](#sec-links)

---

<a id="sec-intro"></a>
## 👋 Introduction

Software development demands consistency, reproducibility and portability across different environments. Docker provides a containerization solution that encapsulates applications and their dependencies into isolated, portable units that run identically regardless of the host machine.  
This project uses a multi-stage Docker architecture to support both development and production workflows. The setup is designed to:  
- Provide a consistent development environment across all team members.
- Eliminate "works on my machine" issues by containerizing all dependencies.
- Support hot-reload during development for rapid iteration.
- Enable seamless integration with CI/CD pipeline.
- Facilitate cross-compilation for ARM64 targets (Raspberry Pi).
- Optimize production builds through layer caching and multi-stage building.

The containerized approach allows developers to work in an environment identical to production, run tests in isolation, and deploy with confidence knowing the exact runtime behavior. Wheter building locally, running tests or deploying to the hardware, the Docker setup ensures consistency at every stage of development lifecycle.

---

<a id="sec-core-ideas"></a>
## 🧠 Core Ideas

- **To be added:** 

---

<a id="sec-architecture"></a>
## 🏗️ Architecture Overview

(Diagrama que mostra como estão feitas as coisas)

---

<a id="sec-docker-components"></a>
## 🔧 Docker Components

(Explicação em detalhe de cada Dockerfile, proposito, use case, build stages, dependencias)

---

<a id="sec-workflow"></a>
## 🛠️ Development Workflow

(Trabalhar no dev, rebuild depois de mudanças, debug dentro do container, hor-reload??)

---

<a id="sec-deployment"></a>
## 📦 Building & Deployment

(Production build, github pushing)

---

<a id="sec-cicd"></a>
## 🔄 CI/CD Integration

(Como isto se integra com o github Actions)

---

<a id="sec-issues"></a>
## 🚨 Issues Log

This section tracks common issues encountered during development, testing or deployment of the instrument cluster.  
Each issue includes its symptoms, probable cause and recommended fix or workaround.

<!-- <a id="issue-x"></a>

### Issue #x - 
**- Error Example:**

**- Cause:** 

**- Solution:** -->

---

<a id="sec-links"></a>
## 🔗 Links



---

> **Document Version:** 1.0  
  **Last Updated:** 22nd Januray 2026  
  **Contributor:** souzitaaaa
