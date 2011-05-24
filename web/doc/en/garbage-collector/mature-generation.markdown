---
layout: doc_en
title: Mature Generation
previous: Young Generation
previous_url: garbage-collector/young-generation
next: Large Objects
next_url: garbage-collector/large-objects
review: true
---
Mature objects are objects that have been promoted from the Young
generation after living past the promotion threshold 'X'.

Autotune is the mechanism that is used to dynamic adjust the GC cycles before
mature collection occurrs. This can be turned off or a static number may be used
via gc.lifetime. gc.lifetime is used by autotune which sets the initial value.

For more information on configuration variables available read the
[vm configuration source file ](https://github.com/evanphx/rubinius/blob/master/vm/configuration.hpp)

