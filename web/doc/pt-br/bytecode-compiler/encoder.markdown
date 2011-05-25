---
layout: doc_pt_br
title: Encoder Stage
previous: Generator Stage
previous_url: bytecode-compiler/generator
next: Packager Stage
next_url: bytecode-compiler/packager
---

Once the generator has processed the AST, it needs to properly encode
the bytecode. This stage is very simple, and is mostly for
recordkeeping.

The Encoder stage is responsible for two things:

* converting the stream of instructions created in the previous stage
  (when the AST nodes called `bytecode`) into an instruction sequence.
* validating that the stack does not underflow, overflow, or otherwise
  contain faulty semantics.

It performs these steps on the main Generator object, as well as any
child Generators (generators for blocks, methods or other structures
encountered inside of the main body).

Once this stage is complete, it passes the encoded Generator object to
the next stage, the Packager stage.

## Files Referenced

* *lib/compiler/generator.rb*: The `encode` method in the file performs
  the bulk of the work in this stage.

## Customization

Since this stage is so simple, you will not need to customize it. You
might want to instrument it (for instance, for profiling or printing).
To learn more about general-purpose customization, which you can use to
instrument any stage, check out [Customizing the Compiler
Pipeline](/bytecode-compiler/customization/).
