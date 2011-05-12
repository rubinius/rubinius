---
layout: doc_pt_br
title: Running Rubinius
previous: Building
previous_url: getting-started/building
next: Troubleshooting
next_url: getting-started/troubleshooting
---

Once you have followed the steps for building (and possibly installing)
Rubinius, you can verify it is working:

    rbx -v

Rubinius generally works like Ruby from the command-line. For example:

    rbx -e 'puts "Hello!"'

To run a ruby file named 'code.rb':

    rbx code.rb

To run IRB:

    rbx

If you added the Rubinius bin directory to your PATH, Rubinius should perform
just as you would expect from MRI. There are commands for `ruby`, `rake`,
`gem`, `irb`, `ri`, and `rdoc`.

You can add the Rubinius bin directory to your PATH only when you want to use
Rubinius. This way, it will not interfere with your normally installed Ruby
when you do not want to use Rubinius.
