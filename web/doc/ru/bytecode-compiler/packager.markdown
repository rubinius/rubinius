---
layout: doc_ru
title: Упаковщик
previous: Кодировщик
previous_url: bytecode-compiler/encoder
next: Запись
next_url: bytecode-compiler/writer
translated: true

---

Once the Generator has been properly encoded in the Encoder stage,
Rubinius packages the bytecode up as a CompiledCode by creating a new
CompiledCode and setting a number of properties on it.

These properties are available on any CompiledCode. You can retrieve
the CompiledCode from a Ruby Method object by calling `executable` on
it.

* *iseq*: a Tuple containing the raw instruction sequence
* *literals*: a Tuple contaning the literals used in the method.
  Literals are used internally by Rubinius for values like Strings, and
  are used by the `push_literal` and `set_literal` instructions.
* *lines*: an Array containing the first instruction pointer for each
  line represented by the bytecode
* *required_args*: the number of arguments required by the method
* *total_args*: the total number of arguments, including optional
  arguments but not `*args`
* *splat*: the position of the splat argument, if any
* *local_count*: the number of local variables, including parameters
* *local_names*: a Tuple containing a list of all of the local variable
  names. The first names will be the required, optional, splat and block
  arguments, in that order.
* *file*: the name of the file that will be used in stack traces and
  other debugging information
* *name*: the name of the method
* *primitive*: the name of the primitive associated with this method, if
  any
* metadata: it is possible to store additional structured metadata on a
  compiled method. The compiled method has a piece of metadata named
  `for_block` with the value `true` if the original generator was
  created for a block.

The Packager stage also makes sure that any child generators (such as
generators for blocks or methods) are also converted into compiled
methods. These child compiled methods are included in the literals tuple
of the parent compiled method.

Once the Generator has finished packaging itself as a CompiledCode, it
invokes the Writer stage, passing in the CompiledCode as its input.

## Files Referenced

* *kernel/bootstrap/compiled_code.rb*: the basic implementation of
  CompiledCode, mostly composed of wiring up primitives
* *kernel/common/compiled_code.rb*: a more robust implementation of
  CompiledCode, a combination of primitive methods and methods written
  in Ruby
* *vm/builtin/compiledcode.cpp*: the C++ implementation of the
  CompiledCode primitives
* *lib/compiler/generator.rb*: The implementation of the `package`
  method, which populates the CompiledCode with information from the
  Generator object.

## Customization

In general, the `package` method is designed to populate the
CompiledCode with a group of variables. However, you could also use
the packager to populate another object with the same interface.
However, it would not necessarily be useful on its own, without
additional customizations later on
