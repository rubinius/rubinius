---
layout: doc_pl
title: Specyfikacja
previous: Zmienne globalne
previous_url: ruby/global-variables
next: RubySpec
next_url: specs/rubyspec
---

The Rubinius project generally uses TDD/BDD-style executable specifications to
drive development. The Rubinius 'spec' directory is conceptually divided into
two parts:

  1. All the files under './spec/ruby' that describe the behavior of MatzRuby.
  2. And all the other files under the './spec' directory that describe the
     behavior of Rubinius.

The specs at ./spec/ruby are a copy of RubySpec at a particular revision.
These are regularly imported from the RubySpec project and failing specs are
tagged so that the CI process always runs a known good set of specs. This
enables easily confirming that changes to Rubinius code do not cause
regressions.

Documentation about the organization of specs and guidelines for writing specs
can be found at the [RubySpec project](http://rubyspec.org/).

Use the following workflow when adding specs and code to Rubinius:

  1. Write failing specs for some aspect of Ruby behavior. Commit the specs in
     a separate commit to the appropriate files under ./spec/ruby.
  2. Add Rubinius code to make the specs pass. Again, commit these changes in
     a separate commit from the spec changes.
  3. Run the `rake` command to ensure that all the CI specs pass.

The changes to files in ./spec/ruby are regularly pushed to the RubySpec
project. Also, current changes to RubySpec from committers to other Ruby
implementations are regularly updated in the ./spec/ruby directories. When the
specs are updated from RubySpec, the CI tags are also updated.
