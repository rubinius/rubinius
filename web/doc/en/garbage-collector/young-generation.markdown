---
layout: doc_en
title: Young Generation
previous: Nursery
previous_url: garbage-collector/nursery
next: Mature Generation
next_url: garbage-collector/mature-generation
review: true
---
Objects that have been alive for more than one GC cycle live in the Young
generation space.  A run of the GC handles moving objects into the Young
generation space automatically.

Objects live in the young generation space for X number of collections.
X is dynamically adjusted based on how many objects are being promoted.

If *too many* objects are promoted during a given GC cycle, then X goes up.
If *not enough* objects are promoted during a given GC cycle, then X goes down.

This is done in order to keep the rate of promotion stable.
*stable* in this context means a low rate of change in object promotion.

