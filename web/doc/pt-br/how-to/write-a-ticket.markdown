---
layout: doc_pt_br
title: How-To - Write a Ticket
previous: How-To
previous_url: how-to
next: Write a Ruby Spec
next_url: how-to/write-a-ruby-spec
---

The Rubinius issue tracker is <http://github.com/evanphx/rubinius/issues>.

To be useful, tickets must be concise, focused, and actionable. If not, the
ticket will languish and become clutter. Accordingly, tickets must fall into
one (or more) of the following categories:

  1. A precise command line history showing how to install and invoke the
     program and showing the backtrace for an exception.
  2. A short piece of code illustrating the problem and the command line to
     invoke it.
  3. A patch, **including specs if they do not already exist**, and showing
     the spec runs before and after applying the patch.

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
     2. Read [Troubleshooting](/doc/pt-br/getting-started/troubleshooting)
        to see if something there resolves the issue.
     3. Read [Specs](/doc/pt-br/specs/).

  2. Give your ticket a specific, preferably short title.

  3. Give your ticket appropriate tags.

  4. Give enough detail about the issue.

     *  The command line for invoking the program
     *  The backtrace or result from the program versus expected result.
     *  Your machine information. `uname -a` is usually good (if there are any
        "unknown" fields in it, please elaborate on those.)


## Additional instructions for tickets with patches

  *  Can be just a set of specs.
  *  Patches must be accompanied by specs unless the specs already exist.
  *  Relevant part of spec output and the exact 'bin/mspec' invocation from the
     existing or added spec *before the fix*.
  *  The spec output and the exact 'bin/mspec' invocation  showing success
     *after* the fix.
  *  Additional description of your patch and how it fixes the problem. In
     particular with new functionality please indicate if it was already
     discussed on #rubinius or ruby-dev.

Unless for some reason impossible, please use 'git-format-patch' to create the
patchset. It is much easier to apply and it preserves the correct attribution.
Otherwise, a unified diff.


## Example of submitting a patch

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
   function.  This patch does so.

   After the patch is applied:

       $ bin/mspec spec/ruby/core/kernel/format_spec.rb
       Started
       ..

       Finished in 0.016031 seconds

       2 examples, 2 expectations, 0 failures, 0 errors

4. Attachment:

Finally, put your patch in a gist and add the link to the gist to your issue.
Below the patch is reproduced inline for completeness:

    From c61cecce6442347ebbdf1ded7a5c0832c97582c1 Mon Sep 17 00:00:00 2001
    From: Brian Ford <bford@engineyard.com>
    Date: Sat, 19 Jan 2008 17:48:19 -0800
    Subject: [PATCH] Set Kernel#format as a module function.


    diff --git a/kernel/core/kernel.rb b/kernel/core/kernel.rb
    index 2d2e508..f2a382e 100644
    --- a/kernel/core/kernel.rb
    +++ b/kernel/core/kernel.rb
    @@ -150,6 +150,7 @@ module Kernel
       end
       alias_method :format, :sprintf
       module_function :sprintf
    +  module_function :format
       module_function :abort

       def puts(*a)

