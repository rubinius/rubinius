---
layout: doc_en
title: How-To - Write a Ticket
previous: How-To
previous_url: how-to
next: Write a Ruby Spec
next_url: how-to/write-a-ruby-spec
---

The Rubinius issue tracker is <http://github.com/rubinius/rubinius/issues>.

To be useful, tickets must be concise, focused, and actionable. If not, the
ticket will languish and become clutter. Accordingly, tickets must fall into
one (or more) of the following categories:

  1. A precise command line history showing how to install and invoke the
     program and showing the backtrace for an exception.
  2. A short piece of code illustrating the problem and the command line to
     invoke it.
  3. A pull request, **including specs if they do not already exist**, and showing
     the spec runs before and after applying the pull request.

If your issue doesn't fit into one of the categories, it is not invalid. It is
simply not appropriate for a ticket.

  1. If it is a feature, consider discussing it on the mailing list. Also, you
     could take a crack at implementing it and demonstrate how your feature is
     useful.
  2. If it is a library or gem that is not working, take some time to dig in
     and see if you can create a reproduction for an issue and post that as a
     ticket.


## General procedure for submitting a ticket

  1. Double-check.

     1. Do a full rebuild ('rake clean; rake') after a 'git pull' or fresh clone.
     2. Read [Troubleshooting](/doc/en/getting-started/troubleshooting)
        to see if something there resolves the issue.
     3. Read [Specs](/doc/en/specs/).

  2. Give your ticket a specific, preferably short title.

  3. Give your ticket appropriate tags.

  4. Give enough detail about the issue.

     *  The command line for invoking the program
     *  The backtrace or result from the program versus expected result.
     *  Your machine information. `uname -a` is usually good (if there are any
        "unknown" fields in it, please elaborate on those.)


## Additional instructions for pull requests

  *  Can be just a set of specs.
  *  Pull requests must be accompanied by specs unless the specs already exist.
  *  Relevant part of spec output and the exact 'bin/mspec' invocation from the
     existing or added spec *before the fix*.
  *  The spec output and the exact 'bin/mspec' invocation  showing success
     *after* the fix.
  *  Additional description of your pull request and how it fixes the problem. In
     particular with new functionality please indicate if it was already
     discussed on #rubinius or ruby-dev.

Unless for some reason impossible, please use a pull request to create the
patchset. It is the easiest way for us to review and apply the changes.
If opening a pull request is impossible, please use git-format-patch and
add a link to it using gist.github.com, pastie.org or a similar service.


## Example of submitting a pull request

Suppose the following spec exists and is failing:

    describe "Kernel.format" do
      it "is accessible as a module function" do
        Kernel.format("%s", "hello").should == "hello"
      end
    end

1. Ticket Title:

   "[PATCH] No method 'format' on Kernel (Module)"

2. Tags:

   "patch core spec"

3. Ticket Message:

   The method 'format' is not available as a module function of Kernel.

       $ bin/mspec spec/ruby/core/kernel/format_spec.rb
       Started
       .E

       1)
       Kernel.format is accessible as a module function ERROR
       No method 'format' on Kernel (Module):

   The method 'format' already exists but has not been set as a module
   function. This pull request does so.

   After the pull request is applied:

       $ bin/mspec spec/ruby/core/kernel/format_spec.rb
       Started
       ..

       Finished in 0.016031 seconds

       2 examples, 2 expectations, 0 failures, 0 errors
