---
layout: doc_en
title: Contributing
previous: Troubleshooting
previous_url: getting-started/troubleshooting
next: Communication
next_url: contributing/communication
---

The Rubinius project welcomes your contributions. There are many things to do
to help out. Most importantly, you should do something that interests you.
That is the best way to have enthusiasm and energy for your work.

If you have questions about Rubinius, the best way to get answers is to chat
with us in the #rubinius IRC channel on irc.freenode.net.

Below are several ideas for things to do on Rubinius.


## Run Your Code

Your code is often more vicious than the specs. Run your pet project under
Rubinius and report issues. See [How To Write a Ticket](/doc/en/how-to/write-a-ticket).


## Ask For Help

Anything that we can do to help, we will. Make sure to do your own research
too, if possible. Rubinius attempts to be a project that you can easily study,
learn from, and hopefully extend.

We will certainly accept and appreciate simple bug reports, but we can give
priority to tickets that include simple steps to reproduce the issue. Even
better are tickets that include RubySpecs that demonstrate the bug and a patch
that fixes it.


## Write Specs

  1. Run `bin/mspec tag --list incomplete <dir>` to show specs that have been
     tagged as incomplete. These specs may simply need review, or there could
     be specs missing for a particular class.

     NOTE: You can specify the pseudo-directory ':files' for \<dir\>, which will
     show tags for all the specs that should run on Rubinius. Or you can
     specify any subdirectory of the spec/ directory to list tags for specs in
     that subdirectory.

  2. Find unspecified behaviors. See [How To Write a Ruby
     Spec](/doc/en/how-to/write-a-ruby-spec).


## Fix Failing Specs

  1. Run `bin/mspec tag --list fails <dir>` to show specs tagged as failing.

     NOTE: You can specify the pseudo-directory ':files' for \<dir\>, which will
     show tags for all the specs that should run on Rubinius. Or you can
     specify any subdirectory of the spec/ directory to list tags for specs in
     that subdirectory.

  2. Pick a spec that looks interesting and see if you can write a patch that
     will make it pass.


## Write Docs

Study how Rubinius works and write highlevel documentation that will help
others learn about the implementation details.


## Cleanup Code

Review the [Style Guide](/doc/en/contributing/style-guide/) for coding
guidelines.


## Triage Tickets

  * Revive or close old tickets.
  * Build minimal test cases that reproduce the bugs. Then see if there are
    already RubySpecs for the issue. If not, consider writing some.
