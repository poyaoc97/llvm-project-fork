// Test -fsplit-machine-functions option pass-through with lto
// RUN: %clang -### -target x86_64-unknown-linux -flto -fsplit-machine-functions %s 2>&1 | FileCheck %s -check-prefix=CHECK-PASS

// Test no pass-through to ld without lto
// RUN: %clang -### -target x86_64-unknown-linux -fsplit-machine-functions %s 2>&1 | FileCheck %s -check-prefix=CHECK-NOPASS

// Test the mix of -fsplit-machine-functions and -fno-split-machine-functions
// RUN: %clang -### -target x86_64-unknown-linux -flto -fsplit-machine-functions -fno-split-machine-functions %s 2>&1 | FileCheck %s -check-prefix=CHECK-NOPASS
// RUN: %clang -### -target x86_64-unknown-linux -flto -fno-split-machine-functions -fsplit-machine-functions %s 2>&1 | FileCheck %s -check-prefix=CHECK-PASS
// Check that for non-X86, passing no-split-machine-functions does not cause error.
// RUN: %clang -### -target aarch64-unknown-linux -flto -fsplit-machine-functions -fno-split-machine-functions %s 2>&1 | FileCheck %s -check-prefix=CHECK-NOPASS2

// CHECK-PASS:          "-plugin-opt=-split-machine-functions"
// CHECK-NOPASS-NOT:    "-plugin-opt=-split-machine-functions"
// CHECK-NOPASS2-NOT:   "-plugin-opt=-split-machine-functions"
