#ifdef ENABLE_LLVM

/*

The code for this memory manager is based of the default jit memory manager
from the LLVM project. This falls under the LLVM license as described here.

==============================================================================
LLVM Release License
==============================================================================
University of Illinois/NCSA
Open Source License

Copyright (c) 2003-2012 University of Illinois at Urbana-Champaign.
All rights reserved.

Developed by:

    LLVM Team

    University of Illinois at Urbana-Champaign

    http://llvm.org

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal with
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimers.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimers in the
      documentation and/or other materials provided with the distribution.

    * Neither the names of the LLVM Team, University of Illinois at
      Urbana-Champaign, nor the names of its contributors may be used to
      endorse or promote products derived from this Software without specific
      prior written permission.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
SOFTWARE.

*/

#include "config.h"

#include <llvm/ExecutionEngine/JITMemoryManager.h>
#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/ADT/Statistic.h>
#include <llvm/ADT/Twine.h>
#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/GlobalValue.h>
#else
#include <llvm/GlobalValue.h>
#endif
#include <llvm/Support/Allocator.h>
#include <llvm/Support/Compiler.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Memory.h>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Config/config.h>
#include <vector>
#include <cassert>
#include <climits>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include "llvm/jit_memory_manager.hpp"

//===----------------------------------------------------------------------===//
// Memory Block Implementation.
//===----------------------------------------------------------------------===//

namespace rubinius {
namespace jit {

  using namespace llvm;


/// AllocateBlock - Mark this entire block allocated, updating freelists
/// etc.  This returns a pointer to the circular free-list.
FreeRangeHeader *FreeRangeHeader::AllocateBlock() {
  assert(!ThisAllocated && !getBlockAfter().PrevAllocated &&
         "Cannot allocate an allocated block!");
  // Mark this block allocated.
  ThisAllocated = 1;
  getBlockAfter().PrevAllocated = 1;

  // Remove it from the free list.
  return RemoveFromFreeList();
}

/// FreeBlock - Turn an allocated block into a free block, adjusting
/// bits in the object headers, and adding an end of region memory block.
/// If possible, coalesce this block with neighboring blocks.  Return the
/// FreeRangeHeader to allocate from.
FreeRangeHeader *MemoryRangeHeader::FreeBlock(FreeRangeHeader *FreeList) {
  MemoryRangeHeader *FollowingBlock = &getBlockAfter();
  assert(ThisAllocated && "This block is already free!");
  assert(FollowingBlock->PrevAllocated && "Flags out of sync!");

  FreeRangeHeader *FreeListToReturn = FreeList;

  // If the block after this one is free, merge it into this block.
  if (!FollowingBlock->ThisAllocated) {
    FreeRangeHeader &FollowingFreeBlock = *(FreeRangeHeader *)FollowingBlock;
    // "FreeList" always needs to be a valid free block.  If we're about to
    // coalesce with it, update our notion of what the free list is.
    if (&FollowingFreeBlock == FreeList) {
      FreeList = FollowingFreeBlock.Next;
      FreeListToReturn = 0;
      assert(&FollowingFreeBlock != FreeList && "No tombstone block?");
    }
    FollowingFreeBlock.RemoveFromFreeList();

    // Include the following block into this one.
    BlockSize += FollowingFreeBlock.BlockSize;
    FollowingBlock = &FollowingFreeBlock.getBlockAfter();

    // Tell the block after the block we are coalescing that this block is
    // allocated.
    FollowingBlock->PrevAllocated = 1;
  }

  assert(FollowingBlock->ThisAllocated && "Missed coalescing?");

  if (FreeRangeHeader *PrevFreeBlock = getFreeBlockBefore()) {
    PrevFreeBlock->GrowBlock(PrevFreeBlock->BlockSize + BlockSize);
    return FreeListToReturn ? FreeListToReturn : PrevFreeBlock;
  }

  // Otherwise, mark this block free.
  FreeRangeHeader &FreeBlock = *(FreeRangeHeader*)this;
  FollowingBlock->PrevAllocated = 0;
  FreeBlock.ThisAllocated = 0;

  // Link this into the linked list of free blocks.
  FreeBlock.AddToFreeList(FreeList);

  // Add a marker at the end of the block, indicating the size of this free
  // block.
  FreeBlock.SetEndOfBlockSizeMarker();
  return FreeListToReturn ? FreeListToReturn : &FreeBlock;
}

/// GrowBlock - The block after this block just got deallocated.  Merge it
/// into the current block.
void FreeRangeHeader::GrowBlock(uintptr_t NewSize) {
  assert(NewSize > BlockSize && "Not growing block?");
  BlockSize = NewSize;
  SetEndOfBlockSizeMarker();
  getBlockAfter().PrevAllocated = 0;
}

/// TrimAllocationToSize - If this allocated block is significantly larger
/// than NewSize, split it into two pieces (where the former is NewSize
/// bytes, including the header), and add the new block to the free list.
FreeRangeHeader *MemoryRangeHeader::
TrimAllocationToSize(FreeRangeHeader *FreeList, uint64_t NewSize) {
  assert(ThisAllocated && getBlockAfter().PrevAllocated &&
         "Cannot deallocate part of an allocated block!");

  // Don't allow blocks to be trimmed below minimum required size
  NewSize = std::max<uint64_t>(FreeRangeHeader::getMinBlockSize(), NewSize);

  // Round up size for alignment of header.
  unsigned HeaderAlign = __alignof(FreeRangeHeader);
  NewSize = (NewSize+ (HeaderAlign-1)) & ~(HeaderAlign-1);

  // Size is now the size of the block we will remove from the start of the
  // current block.
  assert(NewSize <= BlockSize &&
         "Allocating more space from this block than exists!");

  // If splitting this block will cause the remainder to be too small, do not
  // split the block.
  if (BlockSize <= NewSize+FreeRangeHeader::getMinBlockSize())
    return FreeList;

  // Otherwise, we splice the required number of bytes out of this block, form
  // a new block immediately after it, then mark this block allocated.
  MemoryRangeHeader &FormerNextBlock = getBlockAfter();

  // Change the size of this block.
  BlockSize = NewSize;

  // Get the new block we just sliced out and turn it into a free block.
  FreeRangeHeader &NewNextBlock = (FreeRangeHeader &)getBlockAfter();
  NewNextBlock.BlockSize = (char*)&FormerNextBlock - (char*)&NewNextBlock;
  NewNextBlock.ThisAllocated = 0;
  NewNextBlock.PrevAllocated = 1;
  NewNextBlock.SetEndOfBlockSizeMarker();
  FormerNextBlock.PrevAllocated = 0;
  NewNextBlock.AddToFreeList(FreeList);
  return &NewNextBlock;
}

//===----------------------------------------------------------------------===//
// Memory Block Implementation.
//===----------------------------------------------------------------------===//

MemSlab *JITSlabAllocator::Allocate(size_t Size) {
  sys::MemoryBlock B = JMM.allocateNewSlab(Size);
  MemSlab *Slab = (MemSlab*)B.base();
  Slab->Size = B.size();
  Slab->NextPtr = 0;
  return Slab;
}

void JITSlabAllocator::Deallocate(MemSlab *Slab) {
  sys::MemoryBlock B(Slab, Slab->Size);
  sys::Memory::ReleaseRWX(B);
}

RubiniusJITMemoryManager::RubiniusJITMemoryManager()
  :
    LastSlab(0, 0),
    BumpSlabAllocator(*this),
    StubAllocator(DefaultSlabSize, DefaultSizeThreshold, BumpSlabAllocator),
    DataAllocator(DefaultSlabSize, DefaultSizeThreshold, BumpSlabAllocator),
#ifdef NDEBUG
    PoisonMemory(false) {
#else
    PoisonMemory(true) {
#endif

  // Allocate space for code.
  sys::MemoryBlock MemBlock = allocateNewSlab(DefaultCodeSlabSize);
  CodeSlabs.push_back(MemBlock);
  uint8_t *MemBase = (uint8_t*)MemBlock.base();

  // We set up the memory chunk with 4 mem regions, like this:
  //  [ START
  //    [ Free      #0 ] -> Large space to allocate functions from.
  //    [ Allocated #1 ] -> Tiny space to separate regions.
  //    [ Free      #2 ] -> Tiny space so there is always at least 1 free block.
  //    [ Allocated #3 ] -> Tiny space to prevent looking past end of block.
  //  END ]
  //
  // The last three blocks are never deallocated or touched.

  // Add MemoryRangeHeader to the end of the memory region, indicating that
  // the space after the block of memory is allocated.  This is block #3.
  MemoryRangeHeader *Mem3 = (MemoryRangeHeader*)(MemBase+MemBlock.size())-1;
  Mem3->ThisAllocated = 1;
  Mem3->PrevAllocated = 0;
  Mem3->BlockSize     = sizeof(MemoryRangeHeader);

  /// Add a tiny free region so that the free list always has one entry.
  FreeRangeHeader *Mem2 =
    (FreeRangeHeader *)(((char*)Mem3)-FreeRangeHeader::getMinBlockSize());
  Mem2->ThisAllocated = 0;
  Mem2->PrevAllocated = 1;
  Mem2->BlockSize     = FreeRangeHeader::getMinBlockSize();
  Mem2->SetEndOfBlockSizeMarker();
  Mem2->Prev = Mem2;   // Mem2 *is* the free list for now.
  Mem2->Next = Mem2;

  /// Add a tiny allocated region so that Mem2 is never coalesced away.
  MemoryRangeHeader *Mem1 = (MemoryRangeHeader*)Mem2-1;
  Mem1->ThisAllocated = 1;
  Mem1->PrevAllocated = 0;
  Mem1->BlockSize     = sizeof(MemoryRangeHeader);

  // Add a FreeRangeHeader to the start of the function body region, indicating
  // that the space is free.  Mark the previous block allocated so we never look
  // at it.
  FreeRangeHeader *Mem0 = (FreeRangeHeader*)MemBase;
  Mem0->ThisAllocated = 0;
  Mem0->PrevAllocated = 1;
  Mem0->BlockSize = (char*)Mem1-(char*)Mem0;
  Mem0->SetEndOfBlockSizeMarker();
  Mem0->AddToFreeList(Mem2);

  // Start out with the freelist pointing to Mem0.
  FreeMemoryList = Mem0;

}

RubiniusJITMemoryManager::~RubiniusJITMemoryManager() {
  for (unsigned i = 0, e = CodeSlabs.size(); i != e; ++i)
    sys::Memory::ReleaseRWX(CodeSlabs[i]);
}

sys::MemoryBlock RubiniusJITMemoryManager::allocateNewSlab(size_t size) {
  // Allocate a new block close to the last one.
  std::string ErrMsg;
  sys::MemoryBlock *LastSlabPtr = LastSlab.base() ? &LastSlab : 0;
  sys::MemoryBlock B = sys::Memory::AllocateRWX(size, LastSlabPtr, &ErrMsg);
  if (B.base() == 0) {
    report_fatal_error("Allocation failed when allocating new memory in the"
                       " JIT\n" + Twine(ErrMsg));
  }
  LastSlab = B;
  // Initialize the slab to garbage when debugging.
  if (PoisonMemory) {
    memset(B.base(), 0xCD, B.size());
  }
  return B;
}

/// CheckInvariants - For testing only.  Return "" if all internal invariants
/// are preserved, and a helpful error message otherwise.  For free and
/// allocated blocks, make sure that adding BlockSize gives a valid block.
/// For free blocks, make sure they're in the free list and that their end of
/// block size marker is correct.  This function should return an error before
/// accessing bad memory.  This function is defined here instead of in
/// JITMemoryManagerTest.cpp so that we don't have to expose all of the
/// implementation details of RubiniusJITMemoryManager.
bool RubiniusJITMemoryManager::CheckInvariants(std::string &ErrorStr) {
  raw_string_ostream Err(ErrorStr);

  // Construct a the set of FreeRangeHeader pointers so we can query it
  // efficiently.
  llvm::SmallPtrSet<MemoryRangeHeader*, 16> FreeHdrSet;
  FreeRangeHeader* FreeHead = FreeMemoryList;
  FreeRangeHeader* FreeRange = FreeHead;

  do {
    // Check that the free range pointer is in the blocks we've allocated.
    bool Found = false;
    for (std::vector<sys::MemoryBlock>::iterator I = CodeSlabs.begin(),
         E = CodeSlabs.end(); I != E && !Found; ++I) {
      char *Start = (char*)I->base();
      char *End = Start + I->size();
      Found = (Start <= (char*)FreeRange && (char*)FreeRange < End);
    }
    if (!Found) {
      Err << "Corrupt free list; points to " << FreeRange;
      return false;
    }

    if (FreeRange->Next->Prev != FreeRange) {
      Err << "Next and Prev pointers do not match.";
      return false;
    }

    // Otherwise, add it to the set.
    FreeHdrSet.insert(FreeRange);
    FreeRange = FreeRange->Next;
  } while (FreeRange != FreeHead);

  // Go over each block, and look at each MemoryRangeHeader.
  for (std::vector<sys::MemoryBlock>::iterator I = CodeSlabs.begin(),
       E = CodeSlabs.end(); I != E; ++I) {
    char *Start = (char*)I->base();
    char *End = Start + I->size();

    // Check each memory range.
    for (MemoryRangeHeader *Hdr = (MemoryRangeHeader*)Start, *LastHdr = NULL;
         Start <= (char*)Hdr && (char*)Hdr < End;
         Hdr = &Hdr->getBlockAfter()) {
      if (Hdr->ThisAllocated == 0) {
        // Check that this range is in the free list.
        if (!FreeHdrSet.count(Hdr)) {
          Err << "Found free header at " << Hdr << " that is not in free list.";
          return false;
        }

        // Now make sure the size marker at the end of the block is correct.
        uintptr_t *Marker = ((uintptr_t*)&Hdr->getBlockAfter()) - 1;
        if (!(Start <= (char*)Marker && (char*)Marker < End)) {
          Err << "Block size in header points out of current MemoryBlock.";
          return false;
        }
        if (Hdr->BlockSize != *Marker) {
          Err << "End of block size marker (" << *Marker << ") "
              << "and BlockSize (" << Hdr->BlockSize << ") don't match.";
          return false;
        }
      }

      if (LastHdr && LastHdr->ThisAllocated != Hdr->PrevAllocated) {
        Err << "Hdr->PrevAllocated (" << Hdr->PrevAllocated << ") != "
            << "LastHdr->ThisAllocated (" << LastHdr->ThisAllocated << ")";
        return false;
      } else if (!LastHdr && !Hdr->PrevAllocated) {
        Err << "The first header should have PrevAllocated true.";
        return false;
      }

      // Remember the last header.
      LastHdr = Hdr;
    }
  }

  // All invariants are preserved.
  return true;
}

//===----------------------------------------------------------------------===//
// getPointerToNamedFunction() implementation.
//===----------------------------------------------------------------------===//

// AtExitHandlers - List of functions to call when the program exits,
// registered with the atexit() library function.
static std::vector<void (*)()> AtExitHandlers;

/// runAtExitHandlers - Run any functions registered by the program's
/// calls to atexit(3), which we intercept and store in
/// AtExitHandlers.
///
static void runAtExitHandlers() {
  while (!AtExitHandlers.empty()) {
    void (*Fn)() = AtExitHandlers.back();
    AtExitHandlers.pop_back();
    Fn();
  }
}

//===----------------------------------------------------------------------===//
// Function stubs that are invoked instead of certain library calls
//
// Force the following functions to be linked in to anything that uses the
// JIT. This is a hack designed to work around the all-too-clever Glibc
// strategy of making these functions work differently when inlined vs. when
// not inlined, and hiding their real definitions in a separate archive file
// that the dynamic linker can't see. For more info, search for
// 'libc_nonshared.a' on Google, or read http://llvm.org/PR274.
#if defined(__linux__)
/* stat functions are redirecting to __xstat with a version number.  On x86-64
 * linking with libc_nonshared.a and -Wl,--export-dynamic doesn't make 'stat'
 * available as an exported symbol, so we have to add it explicitly.
 */
namespace {
class StatSymbols {
public:
  StatSymbols() {
    sys::DynamicLibrary::AddSymbol("stat", (void*)(intptr_t)stat);
    sys::DynamicLibrary::AddSymbol("fstat", (void*)(intptr_t)fstat);
    sys::DynamicLibrary::AddSymbol("lstat", (void*)(intptr_t)lstat);
    sys::DynamicLibrary::AddSymbol("\x1open64", (void*)(intptr_t)open64);
    sys::DynamicLibrary::AddSymbol("\x1lseek64", (void*)(intptr_t)lseek64);
    sys::DynamicLibrary::AddSymbol("atexit", (void*)(intptr_t)atexit);
    sys::DynamicLibrary::AddSymbol("mknod", (void*)(intptr_t)mknod);
  }
};
}
static StatSymbols initStatSymbols;
#endif // __linux__

// jit_exit - Used to intercept the "exit" library call.
static void jit_exit(int Status) {
  runAtExitHandlers();   // Run atexit handlers...
  exit(Status);
}

// jit_atexit - Used to intercept the "atexit" library call.
static int jit_atexit(void (*Fn)()) {
  AtExitHandlers.push_back(Fn);    // Take note of atexit handler...
  return 0;  // Always successful
}

static int jit_noop() {
  return 0;
}

//===----------------------------------------------------------------------===//
//
/// getPointerToNamedFunction - This method returns the address of the specified
/// function by using the dynamic loader interface.  As such it is only useful
/// for resolving library symbols, not code generated symbols.
///
void *RubiniusJITMemoryManager::getPointerToNamedFunction(const std::string &Name,
                                                         bool AbortOnFailure) {
  // Check to see if this is one of the functions we want to intercept.  Note,
  // we cast to intptr_t here to silence a -pedantic warning that complains
  // about casting a function pointer to a normal pointer.
  if (Name == "exit") return (void*)(intptr_t)&jit_exit;
  if (Name == "atexit") return (void*)(intptr_t)&jit_atexit;

  // We should not invoke parent's ctors/dtors from generated main()!
  // On Mingw and Cygwin, the symbol __main is resolved to
  // callee's(eg. tools/lli) one, to invoke wrong duplicated ctors
  // (and register wrong callee's dtors with atexit(3)).
  // We expect ExecutionEngine::runStaticConstructorsDestructors()
  // is called before ExecutionEngine::runFunctionAsMain() is called.
  if (Name == "__main") return (void*)(intptr_t)&jit_noop;

  const char *NameStr = Name.c_str();
  // If this is an asm specifier, skip the sentinal.
  if (NameStr[0] == 1) ++NameStr;

  // If it's an external function, look it up in the process image...
  void *Ptr = sys::DynamicLibrary::SearchForAddressOfSymbol(NameStr);
  if (Ptr) return Ptr;

  // If it wasn't found and if it starts with an underscore ('_') character,
  // try again without the underscore.
  if (NameStr[0] == '_') {
    Ptr = sys::DynamicLibrary::SearchForAddressOfSymbol(NameStr+1);
    if (Ptr) return Ptr;
  }

  // Darwin/PPC adds $LDBLStub suffixes to various symbols like printf.  These
  // are references to hidden visibility symbols that dlsym cannot resolve.
  // If we have one of these, strip off $LDBLStub and try again.
#if defined(__APPLE__) && defined(__ppc__)
  if (Name.size() > 9 && Name[Name.size()-9] == '$' &&
      memcmp(&Name[Name.size()-8], "LDBLStub", 8) == 0) {
    // First try turning $LDBLStub into $LDBL128. If that fails, strip it off.
    // This mirrors logic in libSystemStubs.a.
    std::string Prefix = std::string(Name.begin(), Name.end()-9);
    if (void *Ptr = getPointerToNamedFunction(Prefix+"$LDBL128", false))
      return Ptr;
    if (void *Ptr = getPointerToNamedFunction(Prefix, false))
      return Ptr;
  }
#endif

  if (AbortOnFailure) {
    report_fatal_error("Program used external function '"+Name+
                      "' which could not be resolved!");
  }
  return 0;
}

// Allocate memory for code in 512K slabs.
const size_t RubiniusJITMemoryManager::DefaultCodeSlabSize = 512 * 1024;

// Allocate globals and stubs in slabs of 64K.  (probably 16 pages)
const size_t RubiniusJITMemoryManager::DefaultSlabSize = 64 * 1024;

const size_t RubiniusJITMemoryManager::DefaultSizeThreshold = 16 * 1024;

}
}

#endif
