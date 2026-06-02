---
id: ASSUMP_L0_7
header: 'Assumption: QT Crosscompilation should be achieved'
level: '1.7'
normative: true
references:
- path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_7/EXPECTATIONS-EXPECT_L0_7.md
  type: file
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_software_dependencies
  configuration:
    packages:
    - GCC
    - ARM64
    - CMake
    - cross-compil
text: GCC ARM64 cross-compiler, CMake, and Qt6 cross-compilation libraries are available
  and configured on the build host before cross-compilation activities begin.
review_status: accepted
---
GCC ARM64 cross-compiler, CMake, and Qt6 cross-compilation libraries are available and configured on the build host before cross-compilation activities begin.

Acceptance criteria / notes:
- GCC/Clang ARM64 cross-compiler is installed and accessible.
- CMake toolchain file is configured with correct sysroot and compiler paths.
- Qt6 cross-compiled libraries are available or will be built as part of setup.

Rationale: Cross-compilation for `EXPECT-L0-7` depends on external toolchain components (GCC, CMake) being available on the build host. This assumption makes toolchain provisioning prerequisites explicit.
