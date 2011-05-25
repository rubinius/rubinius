---
layout: doc_pt_br
title: Writer Stage
previous: Packager Stage
previous_url: bytecode-compiler/packager
next: Transformations
next_url: bytecode-compiler/transformations
---

Once the Packager has created the CompiledMethod, Rubinius will write
the method to a file for future consumption. For instance, after a file
is required for the first time, subsequent requires will load the file
from disk, rather than loading in the Ruby code, then parsing and
compiling it.

This stage is extremely simple. It takes the original input file name,
appends `c` to the end, and calls Rubinius::CompiledFile.dump with the
CompiledMethod from the previous stage and the file name to write to.

Once it finishes writing the file to the disk, it returns the input (the
CompiledMethod), which becomes the return value of the entire compiler
process.

## Files Referenced

* *lib/compiler/compiled_file.rb*: the CompiledFile implementation.
  `CompiledFile.dump` is called in order to perform the actual dumping.

## Customizing

This stage is actually optional, and is only used when compiling a file.
When compiling a String, such as with eval, this step is skipped. In
that case, the compiler stops in the Packager stage, and returns the
CompiledMethod from that stage as the return value of the compiler.

Because of the architecture of the Rubinius compiler, it is easy to add
additional stages to the end of the process, and as long as each of
those stages returns the inputted CompiledMethod (or a different
CompiledMethod), everything will work as expected.

For more information, please read [Customizing the Compiler
Pipeline](/bytecode-compiler/customization/).
