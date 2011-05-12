---
layout: doc_pt_br
title: Global Variables
previous: Class Variables
previous_url: ruby/class-variables
next: Specs
next_url: specs
review: true
---

Syntatically, a global variable is supposed to be a variable whose name begins
with `$`. Global variables are supposed to be available from any context in a
Ruby program. However, there are actually three different kinds of globals: true
globals, thread-local globals, and pseudo globals.

True globals associate a value with a global name, such as `$LOAD_PATH`.

Thread-local globals have the syntax of a global variable but there is a
different version of the global for each thread in the running Ruby process.
Examples of thread-local globals are `$SAFE` and `$!`. To see that these
values depend on the thread, consider the following code:

    puts $SAFE

    Thread.new do
      $SAFE = 2
      puts $SAFE
    end

    puts $SAFE

Pseudo globals are a strict subset of names which refer not to global values
but values in the current scope, like local variables. These are still
referred to as global variables because they start with a dollar sign; this is
a source of confusion for users.

All pseudo globals are organized around one primary pseudo global: `$~`.  They
are all accessing parts of `$~` and thus when `$~` changes, they all
change.

The rest of the pseudo globals are: `$&`, <code>$`</code> (backtick), `$'` (single
quote), `$+`, and `$1`, `$2`, `$3`, etc.

One tricky aspect of these values is that they are strictly bound to the
current scope but Ruby allows them to be aliased, as is done in English.rb.

These new aliases are basically adding new local variables to all scopes, even
ones already running. Thusly, Rubinius does not fully support them. Instead we
simply provide the aliases present in English.rb by default. For example,
`$MATCH` can be used instead `$~`, whether English.rb is included or not.
