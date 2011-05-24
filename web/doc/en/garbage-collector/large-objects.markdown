---
layout: doc_en
title: Large Objects
previous: Mature Generation
previous_url: garbage-collector/mature-generation
next: Rubinius Systems
next_url: systems
review: true
---
Large objects are born in a birthing tank rather than in the nursery.

Any object that exceedes the defined byte size for a large object upon
allocation is allocated directly into the large objects space.  This is done to
avoid copying since copying large objects is an expensive operation.

Note that you can tune the number of bytes that constitute a *large
object* which by default is 2700 (such a good baud rate eh?) bytes.

    rbx ... -Xgc.large_object=<number of bytes> ...

For more information on configuration variables available read the
[vm configuration source file ](https://github.com/evanphx/rubinius/blob/master/vm/configuration.hpp)

