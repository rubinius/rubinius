---
layout: doc_en
title: How-To - Fix a Failing Spec
previous: Write a Ruby Spec
previous_url: how-to/write-a-ruby-spec
next: Write Benchmarks
next_url: how-to/write-benchmarks
---

Make sure you have read:

  *  [Getting Started](/doc/en/getting-started/)
  *  [Specs](/doc/en/specs/)

Then, follow these steps to fix a failing spec:

  1.  Run `rake` to ensure that all CI specs are passing.
  2.  Run `bin/mspec spec/some/spec_file.rb` to confirm the spec fails.
  3.  Edit a file somewhere in Rubinius (probably under the kernel directory).
  4.  Run `rake build` to build your change.
  5.  Run `bin/mspec spec/some/spec_file.rb` to see if your change makes the
      spec pass.
  6.  Repeat until your spec passes.
  7.  Run `rake` to ensure there are no regressions.
  8.  Change directory to Rubinius root if not already there.
  9.  Run `git status, git add, git commit`, etc. Any changes made to the spec
      files under the spec/ruby directory must be in a different commit from
      changes made to other Rubinius source code files.
  10. Run `git format-patch origin`, which will extract commits that the current
      branch accumulated since the last pull from origin, or `git format-patch
      -N', where N is the number (1, 2, etc.) of commits for which you want to
      generate patches.
  11. Create a gist with your patch and link to it in a ticket on the issue
      tracker at http://github.com/evanphx/rubinius/issues. You can add multiple
      patches to one ticket.

When your patch is accepted by the Rubinius project, you'll get a commit bit
for the Rubinius repository. Let Evan know what your Github username is.
