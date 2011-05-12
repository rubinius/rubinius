---
layout: doc_pt_br
title: Generator
previous: Transformations
previous_url: bytecode-compiler/transformations
next: Customizing the Compiler Pipeline
next_url: bytecode-compiler/customization
review: true
---

Once Melbourne has created an AST, it invokes the generator stage,
passing the AST as the input.

The generator stage creates a new instance of `Rubinius::Generator`, and
asks the root of the AST to generate its bytecode onto the `Generator`
object.

A `Generator` provides a pure-Ruby DSL for generating Rubinius bytecode.
At its core, the `Generator` provides methods that map directly to
[Rubinius Instructions](/doc/pt-br/virtual-machine/instructions/). For instance,
to create an instruction to push a nil onto the stack, you could call
the method `push_nil` on a `Generator` instance.

The `Generator` class also provides a number of convenience methods that
allow you to generate common patterns of bytecodes without going into
the very low-level details of some areas of the Rubinius instruction
set.

For instance, to send a method directly using Rubinius bytecode, you
would need to create a literal representing the name of the method, and
then call `send_stack` to send the method. In addition, if you wan#ted to
call a private method, you would first need to create a bytecode
specifically allowing private method invocation. If you wanted to invoke
the method `puts` with no arguments, allowing private method
invocations, you would:

    # here, g is a Generator instance
    g.allow_private
    name = find_literal(:puts)
    g.send_stack name, 0

Using the `send` helper, you could do this more simply:

    g.send :puts, 0, true

When generating the bytecode for an AST, Rubinius invokes the method
`bytecode` on each AST node, passing in the current `Generator`
instance. Here's the bytecode method for the `if` node:

    def bytecode(g)
      pos(g)

      done = g.new_label
      else_label = g.new_label

      @condition.bytecode(g)
      g.gif else_label

      @body.bytecode(g)
      g.goto done

      else_label.set!
      @else.bytecode(g)

      done.set!
    end

First, the method calls the method `pos`, a method on the base `Node`
class, which calls `g.set_line @line`. This is used by the VM to provide
debugging information for running code.

Next, the code uses the label helpers in the `Generator`. Raw Rubinius
bytecode does not have any control structures except for a few goto
instructions (plain `goto`, `goto_if_true` and `goto_if_false`). You can
use the shortened form `git` for `goto_if_true` and `gif` for
`goto_if_false`. In this case, we create two new labels, one for the end
of the `if` condition, and one marking the start of the `else` block.

After creating the two labels, the `if` node invokes the `bytecode`
method on its `@condition` child node, passing along the current
`Generator` object. This will emit the bytecode for the condition into
the current stream.

That process should leave the value of the condition expression on the
stack, so the `if` node emits a `goto_if_false` instruction that will
jump immediately to `else_label`. It then uses the same pattern we saw
before to ask the `@body` to emit its bytecode into the current stream,
and then emits an unconditional `goto` instruction to the end of the
conditional.

Next, we need to mark the location of the `else_label`. By decoupling the
creation of the label from its use, we can pass it to the `goto`
instruction before marking its location, which is crucial for many
control structures.

We then ask the `@else` node to emit its bytecode and mark the location
of the `done` label.

This process occurs recursively from the root node all the way through
the AST, which results in populating the `Generator` object with a
bytecode representation of the AST that started from the root.

You will probably find it useful to look at the classes in the
`lib/compiler/ast` directory, which define all of the AST nodes and
their associated bytecode methods. This is also a good way to see
practical examples of using the `Generator` API.

Once the `Generator` has acquired the bytecode representation of the
AST, it invokes the next stage of the compiler, the Encoder stage.

## Files Referenced

* *lib/compiler/generator_methods.rb*: A generated file containing Ruby
  wrappers around the Rubinius instructions. These methods map directly
  onto the [Rubinius Instructions](/doc/pt-br/virtual-machine/instructions/)
* *lib/compiler/generator.rb*: The definition of the `Generator` object.
  This class mixes in the raw generator methods, and defines a number of
  higher level APIs for generating common bytecode patterns.
* *lib/compiler/ast*: The definition of all of the AST nodes created by
  the Parser stage of the compiler.

## Customization

The easiest way to customize the Generator stage of the compiler process
is by creating higher-level methods in addition to the common ones
provided by the default `Generator` implementation.

You can also customize which generator class is passed in. To learn
more about customizing individual compiler stages or the compiler
pipeline, please read [Customizing the Compiler
Pipeline](/doc/pt-br/bytecode-compiler/customization/).
