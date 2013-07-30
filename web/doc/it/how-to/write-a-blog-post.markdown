---
layout: doc_en
title: How-To - Write a Blog Post
previous: Write Benchmarks
previous_url: how-to/write-benchmarks
next: Write Documentation
next_url: how-to/write-documentation
---

The [Rubinius blog][1] uses [Jekyll][2] and is integrated with the website and
documentation. We encourage and appreciate guest blog posts about your
experiences using or developing Rubinius.

The preferred format for blog posts is Markdown. However, if you have special
formatting needs, the post can be written in HTML directly.

Clone Rubinius repository as that is where the website and posts are stored.

    git clone https://github.com/rubinius/rubinius.git

To get started, ensure you have the `kramdown` and `jekyll` gems installed.

    rbx gem install jekyll kramdown

Now go to the posts directory

    cd rubinius/web/_posts

Create a file in the console using the filename format `YYYY-MM-DD-perma-link.markdown`.

    touch "%(date +"%Y-%m-%d")-perma-link-title.markdown"

Write your brilliant post.

    cat /dev/random > <<the file post>> # :-p

Run jekyll to compile the website, including your post.

    cd ../web/
    rbx -S jekyll build

Create a commit of all the changes in the `web/` directory.

    cd ../
    git add web/
    git commit -m "Wrote a blog post on ....."

Submit a patch, pull request, or if you have commit rights, push the commit to
the master branch.

Tell us that there is a new blog post. We may have some feedback for you before
publishing.

[1]: /blog "Rubinius' Blog"
[2]: https://github.com/mojombo/jekyll "Mojombo's Jekyll"
