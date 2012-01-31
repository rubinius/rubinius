---
layout: post
title: Debugging scary crashes of Rubinius
author: Dirkjan Bussink
---

Bugs, we all are in the infinite fight of getting rid of them. Sometimes
they are trivial, other times a bit harder. In this video I'm going to
show how I have been debugging an issue that caused memory corruption of
your dear Ruby objects in Rubinius.

### Background

It's a pretty long video clocking at 55 minutes but I'm trying to be
detailed in the discovery process. I'm explaining the steps I'm taking
along the way and explaining why I'm taking those.

The real debugging process if of course littered with taking wrong turns
so please don't think I'm brilliant and able to zoom into the issue
immediately all the time. The video is not recorded as I was going
through it the first time, so I exactly knew where I was going and
which steps to take.

If you want to look up some more information on GDB that I'm using
in the video, here are some links that explain some of the most
used commands in the video:

[GDB - Examining data](http://sourceware.org/gdb/onlinedocs/gdb/Data.html)
[GDB - Setting watchpoints](http://sourceware.org/gdb/onlinedocs/gdb/Set-Watchpoints.html)

During the video there's a part from 8:18 until 11:28 where it's just
watching a rebuild of Rubinius scrolling by. I'd suggest skipping it
since it's not very useful. I tried to remove it but after having
to reencode the video it only grew larger. If anyone has more skills
to fix that, please let us know!

### Download

You can find the video on Vimeo:

[Watch on Vimeo](http://vimeo.com/34622441)

It's probably best to download the video and play it locally on your
machine. It's 453 MB so it might take a while.

Please let us know if you find this useful, we might do more of them
in the future if there's interest!


