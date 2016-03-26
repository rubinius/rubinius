//===- llvm/Support/Host.h - Host machine characteristics --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Methods for querying the nature of the host machine.
//
//===----------------------------------------------------------------------===//

#ifndef RBX_LLVM_DETECTION_HPP
#define RBX_LLVM_DETECTION_HPP

#include "config.h"

#include <llvm/Support/Host.h>
#include <llvm/Support/DataStream.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringSwitch.h>

namespace rubinius {

  /// getHostCPUName - Get the LLVM name for the host CPU. The particular format
  /// of the name is target dependent, and suitable for passing as -mcpu to the
  /// target which matches the host.
  ///
  /// \return - The host CPU name, or empty if the CPU could not be determined.
  std::string getHostCPUName();
}

#endif
