---
layout: doc_ja
title: How-To - Write a Ruby Spec
previous: Write a Ticket
previous_url: how-to/write-a-ticket
next: Fix a Failing Spec
next_url: how-to/fix-a-failing-spec
translated: true
---

Make sure you have read:

  *  [Getting Started](/doc/ja/getting-started/)
  *  [Specs](/doc/ja/specs/)

Then, follow these steps to write a spec for a Ruby method:

  1. Edit a file under `spec/ruby/...`
  2. Run `bin/mspec -tr spec/ruby/some/spec_file.rb`
  3. Repeat until the spec passes on MatzRuby
  4. Commit your changes
  7. Use `git format-patch`
  8. Create a gist with your patch and link to it in a ticket on the issue
     tracker at <http://github.com/rubinius/rubinius/issues>.
