---
layout: doc_en
title: Garbage Collector
previous: JIT Compiler
previous_url: jit
next: Nursery
next_url: garbage-collector/nursery
review: true
---

## Introduction

Rubinius implements a [Generational Garbage Collector (GC)](http://en.wikipedia.org/wiki/Garbage_collection_&#40;computer_science&#41;#Generational_GC_.28ephemeral_GC.29)
The Rubinus Generational GC manages the dynamic allocation and deallocation in
the heap space of a running Rubinius process.

## Definitions

1. We say that an object is *alive* if there is at least one structure
(variable) in the interpreter which references the object.
1. When the Garbage Collector (GC) is run, this is called a *collection*.
1. The number of times the GC has been run is called the number of *GC cycles*
   or *GC collections*.

## Object Life Cycle

1. [Nursery](/doc/en/garbage-collector/nursery/)
1. [Young Generation](/doc/en/garbage-collector/young-generation/)
1. [Mature Generation](/doc/en/garbage-collector/mature-generation/)
1. [Large Objects](/doc/en/garbage-collector/large-objects/)
