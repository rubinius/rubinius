---
layout: doc_pl
title: Jak napisać posta na bloga Rubiniusa
previous: How-To - Pisanie testów wydajnościowych
previous_url: how-to/write-benchmarks
next: Jak napisać dokumentację
next_url: how-to/write-documentation
---

The Rubinius blog uses Jekyll and is integrated with the website and
documentation. We encourage and appreciate guest blog posts about your
experiences using or developing Rubinius.

To get started, ensure you have the `kramdown` and `jekyll` gems installed.

    rbx gem install jekyll kramdown

The preferred format for blog posts is Markdown. However, if you have special
formatting needs, the post can be written in HTML directly.

1. Create a file in `web/_posts/` using the format
   `YYYY-MM-DD-perma-link.markdown` for the file name.
1. Write the post.
1. In the `web/` directory, run `rbx -S jekyll build`.
1. Create a commit of all the changes in the `web/` directory.
1. Submit a patch, or if you have commit rights, push the commit to master.
1. Tell us that there is a new blog post. We may have some feedback for you
   before publishing.
