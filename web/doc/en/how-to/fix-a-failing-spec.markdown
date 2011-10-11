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

  1.  Fork the [Rubinius project on GitHub](https://github.com/rubinius/rubinius).
  2.  Clone your fork locally and run `git checkout -b branch_name` to create a new branch (where branch_name is
      a brief description for the problem you are fixing.
  3.  Run `rake` to ensure that all CI specs are passing.
  4.  Run `bin/mspec spec/some/spec_file.rb` to confirm the spec fails.
  5.  Edit a file somewhere in Rubinius (probably under the kernel directory).
  6.  Run `rake build` to build your change.
  7.  Run `bin/mspec spec/some/spec_file.rb` to see if your change makes the
      spec pass.
      Add a "-txVERSION" argument at the end if you'd want to tackle Ruby VERSION. 
      For example: "-tx19" for Ruby 1.9.
  8.  Repeat until your spec passes.
  9.  Run `rake` to ensure there are no regressions.
  10.  Change directory to Rubinius root if not already there.
  11.  Run `git status, git add, git commit`, etc. Any changes made to the spec
      files under the spec/ruby directory must be in a different commit from
      changes made to other Rubinius source code files.
  12. Push your new branch to Github.
  13. Issue a Pull Request.
      To do this, go to your fork on GitHub, change to your new branch and click "Pull Request".
      Next, write a description of your change if required and click "Send pull request".
  14. More commits and discussion may happen with other contributors using
      the Pull Request interface.

When your patch is accepted by the Rubinius project, you'll get a commit bit
for the Rubinius repository. Let Evan know what your Github username is.
