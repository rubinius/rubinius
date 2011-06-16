---
layout: post
title: Rubinius Has a Blog!
author: Brian Ford
---

Many thought the day would never come, but Rubinius finally has a blog. That's
not all, though: We have integrated the website, blog, and documentation using
Jekyll. The source code for it all is in the main [Rubinius
repository](http://github.com/rubinius/rubinius).

People have often requested that we write more about the _awesome_ features in
Rubinius. We hear you and we'd love to do this. However, there is always a
trade-off between working on those awesome features and writing about them.
Until now, it's been rather painful to write docs or blog posts because we did
not have good infrastructure in place. Now, I think we do. I'm sure there are
still a lot of improvements we can make, but we have a good place to start.
I'd like to give a brief tour of our new system.

The primary goal was to improve collaboration and reduce friction for writing
new documentation and blog posts. That's right, improve collaboration. There
are many people who have experience developing Rubinius and running their
applications on it. We love how people have collaborated with source code
commits. Now anyone has the ability to write a blog post as well. I've written
a basic [How-To - Write a Blog Post](/doc/en/how-to/write-a-blog-post/)
document. If you have an idea for a blog post, just let us know. We will
exercise a bit of editorial control just to ensure the topics are appropriate
for Rubinius, but generally, we are thrilled to have your contributions.

Recently, we added the `rbx docs` command. This will run a web server on your
machine and open a browser window to display the Rubinius documentation. Now
the documentation will also be available at the [rubini.us](http://rubini.us)
website. I have added a basic outline and a bunch of files to further simplify
the task of writing docs. In many cases, merely open a file and start writing
docs in [Markdown](http://daringfireball.net/projects/markdown/syntax) format.

We have also begun translating our documentation to other languages. I am
excited about this, being a huge language geek. I wish that I were proficient
in 10 languages so I could polish our documentation for the many people who
are not native English speakers. Alas, I only have a fair ability to write in
Spanish, so we are again depending on your help. I started the translation
effort by passing the existing English docs through Google translate. We have
a beginning guide for [How-To - Translate
Documentation](/doc/en/how-to/translate-documentation). I've been told by
_kronos\_vano_ in our \#rubinius IRC channel that he's already working on a
Russian translation. I personally would love to see Japanese and Chinese
translations.

So that's a brief introduction to our new infrastructure for documenting and
explaining Rubinius. It's been such a joy to see so many people contribute to
the Rubinius source code over the years. We hope that the blog, documentation,
and translations will further empower people to contribute and benefit from
the value that Rubinius has to offer the Ruby community.

_¡Adelante!_
