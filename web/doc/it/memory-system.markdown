---
layout: doc_en
title: Memory system
previous: JIT Compiler
previous_url: jit
next: Object layout
next_url: memory-system/object-layout
review: true
---

## Introduction

In this section the memory system of the Rubinius virtual machine is
described. The first chapter explains how objects are layed out in
memory. The concept of immediates or tagged pointers will be explained,
as well as how we work with types the virtual machine is aware of.
Another important thing to know is how we create compact object so we
use the memory in your system more efficiently. 

The second chapter involves the garbage collector and how we use a 
[Generational Garbage Collector (GC)](http://en.wikipedia.org/wiki/Garbage_collection_&#40;computer_science&#41;#Generational_GC_.28ephemeral_GC.29)
to clean up after the waste your application generates. It will also
explain how we ensure that concurrent allocation of objects performs
well and how you have to consider the implications of a moving garbage
collector when working with the virtual machine code.

1. [Object layout](/doc/en/memory-system/object-layout/)
1. [Garbage collector](/doc/en/memory-system/garbage-collector/)
