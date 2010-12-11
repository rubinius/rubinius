---
layout: doc_en
title: Global Variables
previous: Class Variables
previous_url: ruby/class-variables
next: Specs
next_url: specs
review: true
---

Global variables are variables available from any context. There are actually
two different kinds of globals: True globals and pseudo globals.

True globals associate a value with a global name, such as **$LOAD\_PATH**. Any
value accessed starting with a $ that is not a pseudo global is a true global.

Pseudo globals are a strict subset of names which refer not to global
values but values in the current scope, like local variables. These are
still referred to as global variables because the start with a dollar sign,
but this is the source of confusion for users.

All pseudo globals are organized around one primary pseudo global: **$~**.
They are all simple accessing parts of **$~** and thus when **$~** changes,
they all change.

The rest of the pseudo globals are: **$&**, **$\`** (backtick),
**$'** (single quote), **$+**, and **$1**, **$2**, **$3**, ...

One tricky aspect of these values is that they are strictly bound to the
current scope but Ruby allows them to be aliased, as is done in English.rb.

These new aliases are basically adding new local variables to all scopes, even
ones already running. Thusly, Rubinius does not fully support them. Instead
we simply provide the aliases present in English.rb by default. For example,
**$MATCH** can be used instead **$~**, whether English.rb is included or not.
