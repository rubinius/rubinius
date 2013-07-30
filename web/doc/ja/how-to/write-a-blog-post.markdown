---
layout: doc_ja
title: How-To - Write a Blog Post
previous: Fix a Failing Spec
previous_url: how-to/fix-a-failing-spec
next: Write Documentation
next_url: how-to/write-documentation
translated: true
---

Rubiniusのブログは、Jekyllを使用して、Webサイトと統合されています
マニュアルを参照してください私たちは奨励し、約ゲストブログの記事に感謝を
経験を使用するか、Rubiniusの開発。

開始するには、`kramdown`は、インストール`jekyll`宝石を持っていることを確認します。

    rbx gem install jekyll kramdown

ブログの記事に適した形式はMarkdownです。ただし、特別している場合
ニーズの書式設定、ポストは直接HTMLで記述することができます。

1. Create a file in `web/_posts/` using the format
   `YYYY-MM-DD-perma-link.markdown` for the file name.
1. Write the post.
1. In the `web/` directory, run `rbx -S jekyll build`.
1. Create a commit of all the changes in the `web/` directory.
1. Submit a patch, or if you have commit rights, push the commit to master.
1. Tell us that there is a new blog post. We may have some feedback for you
   before publishing.
