#ifndef RUBINIUS_JIT_MEMORY_BARRIER
#define RUBINIUS_JIT_MEMORY_BARRIER

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


#include <llvm/Support/Memory.h>
#include <llvm/Support/Allocator.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>

#include "util/thread.hpp"

namespace rubinius {
namespace jit {

  using namespace llvm;

  class RubiniusJITMemoryManager;
  class RubiniusRequestJITMemoryManager;

  class JITSlabAllocator {
    RubiniusJITMemoryManager &JMM;
  public:
    JITSlabAllocator(RubiniusJITMemoryManager &jmm) : JMM(jmm) { }
    void *Allocate(size_t Size, size_t /*Alignment*/);
    void Deallocate(void *Slab, size_t Size);
  };

  /// MemoryRangeHeader - For a range of memory, this is the header that we put
  /// on the block of memory.  It is carefully crafted to be one word of memory.
  /// Allocated blocks have just this header, free'd blocks have FreeRangeHeader
  /// which starts with this.
  struct FreeRangeHeader;
  struct MemoryRangeHeader {
    /// ThisAllocated - This is true if this block is currently allocated.  If
    /// not, this can be converted to a FreeRangeHeader.
    unsigned ThisAllocated : 1;

    /// PrevAllocated - Keep track of whether the block immediately before us is
    /// allocated.  If not, the word immediately before this header is the size
    /// of the previous block.
    unsigned PrevAllocated : 1;

    /// BlockSize - This is the size in bytes of this memory block,
    /// including this header.
    uintptr_t BlockSize : (sizeof(intptr_t)*CHAR_BIT - 2);


    /// getBlockAfter - Return the memory block immediately after this one.
    ///
    MemoryRangeHeader &getBlockAfter() const {
      return *(MemoryRangeHeader*)((char*)this+BlockSize);
    }

    /// getFreeBlockBefore - If the block before this one is free, return it,
    /// otherwise return null.
    FreeRangeHeader *getFreeBlockBefore() const {
      if (PrevAllocated) return 0;
      intptr_t PrevSize = ((intptr_t *)this)[-1];
      return (FreeRangeHeader*)((char*)this-PrevSize);
    }

    /// FreeBlock - Turn an allocated block into a free block, adjusting
    /// bits in the object headers, and adding an end of region memory block.
    FreeRangeHeader *FreeBlock(FreeRangeHeader *FreeList);

    /// TrimAllocationToSize - If this allocated block is significantly larger
    /// than NewSize, split it into two pieces (where the former is NewSize
    /// bytes, including the header), and add the new block to the free list.
    FreeRangeHeader *TrimAllocationToSize(FreeRangeHeader *FreeList,
                                          uint64_t NewSize);
  };

  /// FreeRangeHeader - For a memory block that isn't already allocated, this
  /// keeps track of the current block and has a pointer to the next free block.
  /// Free blocks are kept on a circularly linked list.
  struct FreeRangeHeader : public MemoryRangeHeader {
    FreeRangeHeader *Prev;
    FreeRangeHeader *Next;

    /// getMinBlockSize - Get the minimum size for a memory block.  Blocks
    /// smaller than this size cannot be created.
    static unsigned getMinBlockSize() {
      return sizeof(FreeRangeHeader)+sizeof(intptr_t);
    }

    /// SetEndOfBlockSizeMarker - The word at the end of every free block is
    /// known to be the size of the free block.  Set it for this block.
    void SetEndOfBlockSizeMarker() {
      void *EndOfBlock = (char*)this + BlockSize;
      ((intptr_t *)EndOfBlock)[-1] = BlockSize;
    }

    FreeRangeHeader *RemoveFromFreeList() {
      assert(Next->Prev == this && Prev->Next == this && "Freelist broken!");
      Next->Prev = Prev;
      return Prev->Next = Next;
    }

    void AddToFreeList(FreeRangeHeader *FreeList) {
      Next = FreeList;
      Prev = FreeList->Prev;
      Prev->Next = this;
      Next->Prev = this;
    }

    /// GrowBlock - The block after this block just got deallocated.  Merge it
    /// into the current block.
    void GrowBlock(uintptr_t NewSize);

    /// AllocateBlock - Mark this entire block allocated, updating freelists
    /// etc.  This returns a pointer to the circular free-list.
    FreeRangeHeader *AllocateBlock();
  };



  /// RubiniusJITMemoryManager - Manage memory for the JIT code generation.
  /// This splits a large block of MAP_NORESERVE'd memory into two
  /// sections, one for function stubs, one for the functions themselves.  We
  /// have to do this because we may need to emit a function stub while in the
  /// middle of emitting a function, and we don't know how large the function we
  /// are emitting is.
  class RubiniusJITMemoryManager {

    friend class RubiniusRequestJITMemoryManager;

  public:
    /// DefaultCodeSlabSize - When we have to go map more memory, we allocate at
    /// least this much unless more is requested. Currently, in 512k slabs.
    static const size_t DefaultCodeSlabSize = 512 * 1024;

    /// DefaultSlabSize - Allocate globals and stubs into slabs of 64K (probably
    /// 16 pages) unless we get an allocation above SizeThreshold.
    static const size_t DefaultSlabSize = 64 * 1024;

    /// DefaultSizeThreshold - For any allocation larger than 16K (probably
    /// 4 pages), we should allocate a separate slab to avoid wasted space at
    /// the end of a normal slab.
    static const size_t DefaultSizeThreshold = 16 * 1024;

  private:

    /// LastSlab - This points to the last slab allocated and is used as the
    /// NearBlock parameter to AllocateRWX so that we can attempt to lay out all
    /// stubs, data, and code contiguously in memory.  In general, however, this
    /// is not possible because the NearBlock parameter is ignored on Windows
    /// platforms and even on Unix it works on a best-effort pasis.
    sys::MemoryBlock LastSlab;

    // Memory slabs allocated by the JIT.  We refer to them as slabs so we don't
    // confuse them with the blocks of memory described above.
    std::vector<sys::MemoryBlock> CodeSlabs;
    BumpPtrAllocatorImpl<JITSlabAllocator, DefaultSlabSize, DefaultSizeThreshold> StubAllocator;
    BumpPtrAllocatorImpl<JITSlabAllocator, DefaultSlabSize, DefaultSizeThreshold> DataAllocator;

    // Circular list of free blocks.
    FreeRangeHeader *FreeMemoryList;

    // When emitting code into a memory block, this is the block.
    MemoryRangeHeader *CurBlock;

    utilities::thread::SpinLock lock_;

    // Whether to poison freed memory.
    bool PoisonMemory;

  public:
    RubiniusJITMemoryManager();
    virtual ~RubiniusJITMemoryManager();

    /// allocateNewSlab - Allocates a new MemoryBlock and remembers it as the
    /// last slab it allocated, so that subsequent allocations follow it.
    sys::MemoryBlock allocateNewSlab(size_t size);

    /// getPointerToNamedFunction - This method returns the address of the
    /// specified function by using the dlsym function call.
    void *getPointerToNamedFunction(const std::string &Name,
                                            bool AbortOnFailure = true);

    // Testing methods.
    bool CheckInvariants(std::string &ErrorStr);
    size_t GetDefaultCodeSlabSize() { return DefaultCodeSlabSize; }
    size_t GetDefaultDataSlabSize() { return DefaultSlabSize; }
    size_t GetDefaultStubSlabSize() { return DefaultSlabSize; }
    unsigned GetNumCodeSlabs() { return CodeSlabs.size(); }
    unsigned GetNumDataSlabs() { return DataAllocator.GetNumSlabs(); }
    unsigned GetNumStubSlabs() { return StubAllocator.GetNumSlabs(); }

    /// startFunctionBody - When a function starts, allocate a block of free
    /// executable memory, returning a pointer to it and its actual size.
    uint8_t *startFunctionBody(const Function *F, uintptr_t &ActualSize) {
      utilities::thread::SpinLock::LockGuard guard(lock_);

      FreeRangeHeader* candidateBlock = FreeMemoryList;
      FreeRangeHeader* head = FreeMemoryList;
      FreeRangeHeader* iter = head->Next;

      uintptr_t largest = candidateBlock->BlockSize;

      // Search for the largest free block
      while (iter != head) {
        if (iter->BlockSize > largest) {
          largest = iter->BlockSize;
          candidateBlock = iter;
        }
        iter = iter->Next;
      }

      largest = largest - sizeof(MemoryRangeHeader);

      // If this block isn't big enough for the allocation desired, allocate
      // another block of memory and add it to the free list.
      if (largest < ActualSize ||
          largest <= FreeRangeHeader::getMinBlockSize()) {
        candidateBlock = allocateNewCodeSlab((size_t)ActualSize);
      }

      // Select this candidate block for allocation
      CurBlock = candidateBlock;

      // Allocate the entire memory block.
      FreeMemoryList = candidateBlock->AllocateBlock();
      ActualSize = CurBlock->BlockSize - sizeof(MemoryRangeHeader);
      return (uint8_t *)(CurBlock + 1);
    }

    /// allocateNewCodeSlab - Helper method to allocate a new slab of code
    /// memory from the OS and add it to the free list.  Returns the new
    /// FreeRangeHeader at the base of the slab.
    FreeRangeHeader *allocateNewCodeSlab(size_t MinSize) {
      // If the user needs at least MinSize free memory, then we account for
      // two MemoryRangeHeaders: the one in the user's block, and the one at the
      // end of the slab.
      size_t PaddedMin = MinSize + 2 * sizeof(MemoryRangeHeader);
      size_t SlabSize = std::max(DefaultCodeSlabSize, PaddedMin);
      sys::MemoryBlock B = allocateNewSlab(SlabSize);
      CodeSlabs.push_back(B);
      char *MemBase = (char*)(B.base());

      // Put a tiny allocated block at the end of the memory chunk, so when
      // FreeBlock calls getBlockAfter it doesn't fall off the end.
      MemoryRangeHeader *EndBlock =
          (MemoryRangeHeader*)(MemBase + B.size()) - 1;
      EndBlock->ThisAllocated = 1;
      EndBlock->PrevAllocated = 0;
      EndBlock->BlockSize = sizeof(MemoryRangeHeader);

      // Start out with a vast new block of free memory.
      FreeRangeHeader *NewBlock = (FreeRangeHeader*)MemBase;
      NewBlock->ThisAllocated = 0;
      // Make sure getFreeBlockBefore doesn't look into unmapped memory.
      NewBlock->PrevAllocated = 1;
      NewBlock->BlockSize = (uintptr_t)EndBlock - (uintptr_t)NewBlock;
      NewBlock->SetEndOfBlockSizeMarker();
      NewBlock->AddToFreeList(FreeMemoryList);

      assert(NewBlock->BlockSize - sizeof(MemoryRangeHeader) >= MinSize &&
             "The block was too small!");
      return NewBlock;
    }

    /// endFunctionBody - The function F is now allocated, and takes the memory
    /// in the range [FunctionStart,FunctionEnd).
    void endFunctionBody(const Function *F, uint8_t *FunctionStart,
                         uint8_t *FunctionEnd) {
      utilities::thread::SpinLock::LockGuard guard(lock_);
      assert(FunctionEnd > FunctionStart);
      assert(FunctionStart == (uint8_t *)(CurBlock+1) &&
             "Mismatched function start/end!");

      uintptr_t BlockSize = FunctionEnd - (uint8_t *)CurBlock;

      // Release the memory at the end of this block that isn't needed.
      FreeMemoryList =CurBlock->TrimAllocationToSize(FreeMemoryList, BlockSize);
    }

    /// allocateSpace - Allocate a memory block of the given size.  This method
    /// cannot be called between calls to startFunctionBody and endFunctionBody.
    uint8_t *allocateSpace(intptr_t Size, unsigned Alignment) {
      utilities::thread::SpinLock::LockGuard guard(lock_);

      CurBlock = FreeMemoryList;
      FreeMemoryList = FreeMemoryList->AllocateBlock();

      uint8_t *result = (uint8_t *)(CurBlock + 1);

      if (Alignment == 0) Alignment = 1;
      result = (uint8_t*)(((intptr_t)result+Alignment-1) &
               ~(intptr_t)(Alignment-1));

      uintptr_t BlockSize = result + Size - (uint8_t *)CurBlock;
      FreeMemoryList =CurBlock->TrimAllocationToSize(FreeMemoryList, BlockSize);

      return result;
    }

    /// allocateStub - Allocate memory for a function stub.
    uint8_t *allocateStub(const GlobalValue* F, unsigned StubSize,
                          unsigned Alignment) {
      utilities::thread::SpinLock::LockGuard guard(lock_);
      return (uint8_t*)StubAllocator.Allocate(StubSize, Alignment);
    }

    /// allocateGlobal - Allocate memory for a global.
    uint8_t *allocateGlobal(uintptr_t Size, unsigned Alignment) {
      utilities::thread::SpinLock::LockGuard guard(lock_);
      return (uint8_t*)DataAllocator.Allocate(Size, Alignment);
    }

    /// allocateCodeSection - Allocate memory for a code section.
    virtual uint8_t *allocateCodeSection(uintptr_t Size, unsigned Alignment,
                                 unsigned SectionID, StringRef SectionName) {
      utilities::thread::SpinLock::LockGuard guard(lock_);
      // Grow the required block size to account for the block header
      Size += sizeof(*CurBlock);

      // FIXME: Alignement handling.
      FreeRangeHeader* candidateBlock = FreeMemoryList;
      FreeRangeHeader* head = FreeMemoryList;
      FreeRangeHeader* iter = head->Next;

      uintptr_t largest = candidateBlock->BlockSize;

      // Search for the largest free block.
      while (iter != head) {
        if (iter->BlockSize > largest) {
          largest = iter->BlockSize;
          candidateBlock = iter;
        }
        iter = iter->Next;
      }

      largest = largest - sizeof(MemoryRangeHeader);

      // If this block isn't big enough for the allocation desired, allocate
      // another block of memory and add it to the free list.
      if (largest < Size || largest <= FreeRangeHeader::getMinBlockSize()) {
        candidateBlock = allocateNewCodeSlab((size_t)Size);
      }

      // Select this candidate block for allocation
      CurBlock = candidateBlock;

      // Allocate the entire memory block.
      FreeMemoryList = candidateBlock->AllocateBlock();
      // Release the memory at the end of this block that isn't needed.
      FreeMemoryList = CurBlock->TrimAllocationToSize(FreeMemoryList, Size);
      return (uint8_t *)(CurBlock + 1);
    }

    /// allocateDataSection - Allocate memory for a data section.
    /// TODO: currently IsReadOnly is ignored.
    virtual uint8_t *allocateDataSection(uintptr_t Size, unsigned Alignment,
                                 unsigned SectionID, StringRef SectionName,
                                 bool IsReadOnly) {
      utilities::thread::SpinLock::LockGuard guard(lock_);
      return (uint8_t*)DataAllocator.Allocate(Size, Alignment);
    }

    /// startExceptionTable - Use startFunctionBody to allocate memory for the
    /// function's exception table.
    uint8_t* startExceptionTable(const Function* F, uintptr_t &ActualSize) {
      utilities::thread::SpinLock::LockGuard guard(lock_);
      return startFunctionBody(F, ActualSize);
    }

    /// endExceptionTable - The exception table of F is now allocated,
    /// and takes the memory in the range [TableStart,TableEnd).
    void endExceptionTable(const Function *F, uint8_t *TableStart,
                           uint8_t *TableEnd, uint8_t* FrameRegister) {
      utilities::thread::SpinLock::LockGuard guard(lock_);
      assert(TableEnd > TableStart);
      assert(TableStart == (uint8_t *)(CurBlock+1) &&
             "Mismatched table start/end!");

      uintptr_t BlockSize = TableEnd - (uint8_t *)CurBlock;

      // Release the memory at the end of this block that isn't needed.
      FreeMemoryList =CurBlock->TrimAllocationToSize(FreeMemoryList, BlockSize);
    }

    void deallocateBlock(void *Block) {
      // Find the block that is allocated for this function.
      MemoryRangeHeader *MemRange = static_cast<MemoryRangeHeader*>(Block) - 1;
      assert(MemRange->ThisAllocated && "Block isn't allocated!");

      // Fill the buffer with garbage!
      if (PoisonMemory) {
        memset(MemRange+1, 0xCD, MemRange->BlockSize-sizeof(*MemRange));
      }

      // Free the memory.
      FreeMemoryList = MemRange->FreeBlock(FreeMemoryList);
    }

    /// deallocateFunctionBody - Deallocate all memory for the specified
    /// function body.
    void deallocateFunctionBody(void *Body) {
      utilities::thread::SpinLock::LockGuard guard(lock_);
      if(Body) {
        deallocateBlock(Body);
      }
    }

    /// deallocateExceptionTable - Deallocate memory for the specified
    /// exception table.
    void deallocateExceptionTable(void *ET) {
      utilities::thread::SpinLock::LockGuard guard(lock_);
      if (ET) deallocateBlock(ET);
    }

    /// setMemoryWritable - When code generation is in progress,
    /// the code pages may need permissions changed.
    void setMemoryWritable()
    {
      for (unsigned i = 0, e = CodeSlabs.size(); i != e; ++i)
        sys::Memory::setWritable(CodeSlabs[i]);
    }
    /// setMemoryExecutable - When code generation is done and we're ready to
    /// start execution, the code pages may need permissions changed.
    void setMemoryExecutable()
    {
      for (unsigned i = 0, e = CodeSlabs.size(); i != e; ++i)
        sys::Memory::setExecutable(CodeSlabs[i]);
    }

    /// setPoisonMemory - Controls whether we write garbage over freed memory.
    ///
    void setPoisonMemory(bool poison) {
      PoisonMemory = poison;
    }
  };

  /// RubiniusRequestJITMemoryManager - Wrapper for memory manager
  /// for a single request. This is needed because LLVM cleans this object
  /// up when the engine associated is also removed.
    // TODO: LLVM-3.6
  class RubiniusRequestJITMemoryManager { // : public JITMemoryManager {

    friend class RubiniusJITMemoryManager;

    RubiniusJITMemoryManager* mgr_;
    void* GeneratedFunction;
    // TODO: LLVM-3.6
    // uint8_t *GOTBase;     // Target Specific reserved memory

  public:
    RubiniusRequestJITMemoryManager(RubiniusJITMemoryManager* mgr)
      : mgr_(mgr)
      , GeneratedFunction(NULL)
      // TODO: LLVM-3.6
      // , GOTBase(NULL)
    {}

    virtual ~RubiniusRequestJITMemoryManager() {
      // TODO: LLVM-3.6
      // if(GOTBase) delete[] GOTBase;
    }

    void *getPointerToNamedFunction(const std::string &Name,
                                            bool AbortOnFailure = true) {
      return mgr_->getPointerToNamedFunction(Name, AbortOnFailure);
    }

    virtual uint8_t *allocateCodeSection(uintptr_t Size, unsigned Alignment,
                                 unsigned SectionID, StringRef SectionName) {
      return mgr_->allocateCodeSection(Size, Alignment, SectionID,
                                       SectionName);
    }

    /// allocateDataSection - Allocate memory for a data section.
    virtual uint8_t *allocateDataSection(uintptr_t Size, unsigned Alignment,
                                 unsigned SectionID, StringRef SectionName,
                                 bool IsReadOnly) {
      return mgr_->allocateDataSection(Size, Alignment, SectionID,
                                       SectionName, IsReadOnly);
    }

    virtual bool applyPermissions(std::string* ErrMsg = 0) {
      return false;
    };

    void AllocateGOT() {
      // TODO: LLVM-3.6
      // GOTBase = new uint8_t[sizeof(void*) * 8192];
      // HasGOT = true;
    }

    bool CheckInvariants(std::string &ErrorStr) { return mgr_->CheckInvariants(ErrorStr); }
    size_t GetDefaultCodeSlabSize() { return mgr_->DefaultCodeSlabSize; }
    size_t GetDefaultDataSlabSize() { return mgr_->DefaultSlabSize; }
    size_t GetDefaultStubSlabSize() { return mgr_->DefaultSlabSize; }
    unsigned GetNumCodeSlabs() { return mgr_->CodeSlabs.size(); }
    unsigned GetNumDataSlabs() { return mgr_->DataAllocator.GetNumSlabs(); }
    unsigned GetNumStubSlabs() { return mgr_->StubAllocator.GetNumSlabs(); }

    uint8_t *startFunctionBody(const Function *F, uintptr_t &ActualSize) {
      GeneratedFunction = NULL;
      return mgr_->startFunctionBody(F, ActualSize);
    }

    void endFunctionBody(const Function *F, uint8_t *FunctionStart,
                         uint8_t *FunctionEnd) {
      GeneratedFunction = FunctionStart;
      return mgr_->endFunctionBody(F, FunctionStart, FunctionEnd);
    }

    uint8_t *allocateSpace(intptr_t Size, unsigned Alignment) {
      return mgr_->allocateSpace(Size, Alignment);
    }

    uint8_t *allocateStub(const GlobalValue* F, unsigned StubSize,
                          unsigned Alignment) {
      return mgr_->allocateStub(F, StubSize, Alignment);
    }

    uint8_t *allocateGlobal(uintptr_t Size, unsigned Alignment) {
      return mgr_->allocateGlobal(Size, Alignment);
    }

    uint8_t* startExceptionTable(const Function* F, uintptr_t &ActualSize) {
      return mgr_->startExceptionTable(F, ActualSize);
    }

    void endExceptionTable(const Function *F, uint8_t *TableStart,
                           uint8_t *TableEnd, uint8_t* FrameRegister) {
      mgr_->endExceptionTable(F, TableStart, TableEnd, FrameRegister);
    }

    uint8_t *getGOTBase() const {
      // TODO: LLVM-3.6
      // return GOTBase;
      return NULL;
    }

    void deallocateFunctionBody(void *Body) {
      if(GeneratedFunction) {
        mgr_->deallocateFunctionBody(Body);
      }
    }

    void deallocateExceptionTable(void *ET) {
      mgr_->deallocateExceptionTable(ET);
    }

    void setMemoryWritable() { mgr_->setMemoryWritable(); }
    void setMemoryExecutable() { mgr_->setMemoryExecutable(); }

    void setPoisonMemory(bool poison) { mgr_->setPoisonMemory(poison); }

    void* generatedFunction() {
      return GeneratedFunction;
    }

    void resetGeneratedFunction() {
      GeneratedFunction = NULL;
    }

    virtual bool finalizeMemory(std::string* ErrMsg = 0) {
      return false;
    }
  };
}
}

#endif
