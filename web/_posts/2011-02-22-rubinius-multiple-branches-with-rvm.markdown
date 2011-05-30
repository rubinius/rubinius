---
layout: post
title: Running Multiple Rubinius Branches Simultaneously with RVM.
author: Wayne E. Seguin
---

This article is written with the assumption that you have RVM installed
already. If you do not, follow the
[Installation Instructions](https://rvm.beginrescueend.com/rvm/install/)
followed by the
[Basics](https://rvm.beginrescueend.com/rvm/basics/)
closely first.


### Named Ruby Installs

Everyone familiar with RVM knows that it allows you to quickly and
easily install a particular Ruby interpreter by simply running, for
example,

    rvm install rbx

What is not widely known (yet) is that there is a "Named Rubies" feature
that allows you to install altered versions of the same Ruby
installation along side the original.

In the case of Rubinius there is this facinating branch called 'hydra'.
So let us see how we can have the Rubinius master branch installed as
the main rbx with the hydra branch installed along side as well.

As above you first install rbx which is currently defaulted to -head
version so

    rvm install rbx

is currently equivalent to

    rvm install rbx-head

After we have the mainline head Rubinus branch installed, we now want to
use the named rubies feature. This is done using the -n specifier in the
Ruby identifier string. So for example to install our hydra branch as an
RVM ruby with the name 'hydra' in it we do the following:

    rvm install --branch hydra rbx-nhydra

Now we can see that they can be used together! Using the Rubinius master
environment,


    $ rvm rbx ; ruby -v
    rubinius 1.2.1 (1.8.7 6feb585f 2011-02-15 JI) [x86_64-apple-darwin10.6.0]

Whereas using the Rubinius hydra environment,

    $ rvm rbx-nhydra ; ruby -v
    rubinius 1.3.0dev (1.8.7 6feb585f xxxx-xx-xx JI) [x86_64-apple-darwin10.6.0]

We see that the next release of Rubinius (hydra branch) is indeed
version 1.3.0 whereas the master branch is version 1.2.1.

Also please note that RVM creates wrapper scripts, so you do not need to
switch out the entire environment just to run the differen versions
either:

For Rubinius master,

    $ rbx-head -v
    rubinius 1.2.1 (1.8.7 6feb585f 2011-02-15 JI) [x86_64-apple-darwin10.6.0]

For Rubinius hydra,

    $ rbx-head-nhydra -v
    rubinius 1.3.0dev (1.8.7 6feb585f xxxx-xx-xx JI) [x86_64-apple-darwin10.6.0]

There is a lot more available to you than this, for more information on
RVM capabilities please visit the [RVM
Website](https://rvm.beginrescueend.com/) and also come talk to us in #rvm on
irc.freenode.net during the daytime EDT.

I hope that this is helpful and informative to you!

  ~Wayne
