---
layout: post
title: Adam Prescott on Scopes in Rubinius
author: Shane Becker
email: veganstraightedge@gmail.com
---

<cite class="vcard"><a class="url fn" href="http://aprescott.com" title="Adam Prescott">Adam Prescott</a></cite> wrote a lengthy article about <i>variables, closures and scope</i> in Ruby, <a href="http://aprescott.com/posts/variables-closures-and-scope" title="Posts / Variables, closures &amp; scope &mdash; Adam Prescott">mentioning Rubinius at the end</a>:

<blockquote cite="http://aprescott.com/posts/variables-closures-and-scope">
  <p> One of the really cool things about the <a href="http://en.wikipedia.org/wiki/Rubinius">Rubinius</a> implementation of Ruby is that it exposes, by requirement, a level of internals which you can&rsquo;t find in <abbr title="Matz' Ruby Interpreter">MRI</abbr>, including some internals with scopes. Because these internals are exposed in Ruby itself, you can <a href="http://yehudakatz.com/2011/02/18/getting-comfortable-with-rubinius-pure-ruby-internals/">play around with scopes as objects</a>, using <code>VariableScope</code>, including getting access to the available local variables within that scope, with <code>VariableScope.current.locals</code>. </p>
</blockquote>

How are **you** using Rubinius? What are you doing with it that you couldn't do before? What is it missing for you to really dive in? Let us know. We're listening.

* [community@rubini.us](community@rubini.us)
* [@rubinius](http://twitter.com/rubinius "@rubinius on twitter")

&mdash; Use Ruby&trade;
