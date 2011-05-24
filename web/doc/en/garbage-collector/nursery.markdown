---
layout: doc_en
title: Nursery
previous: Garbage Collector
previous_url: garbage-collector
next: Young Generation
next_url: garbage-collector/young-generation
review: true
---
The first generation in Rubinius's generational GC is the nursery.
An object's lifcycle begins in the nursery where it is created (allocated).

Objects live in the nursery until the very next collection. If an object
is still alive after one GC cycle then it is moved to the Young generation.
A run of the GC handles moving objects out of the Nursery automatically.

