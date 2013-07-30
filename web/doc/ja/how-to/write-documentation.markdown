---
layout: doc_ja
title: How-To - Write Documentation
previous: Write a Blog Post
previous_url: how-to/write-a-blog-post
next: Translate Documentation
next_url: how-to/translate-documentation
translated: true
---

Rubiniusのドキュメントは、Webサイトやブログに統合されています。これは、使用して
Jekyllは単に他のコンポーネントと同様に。

開始するには、`kramdown`は、インストール`jekyll`宝石を持っていることを確認します。

    rbx gem install jekyll kramdown

ドキュメントのソースは、`web/doc`のディレクトリの下にあります。があります。
各言語のサブディレクトリは、ドキュメントがされていると
（例：`en`は、`ja`を、など）が翻訳されています。

目次は、例えば各翻訳（のためにそこにいる
`/web/doc/ja/index.markdown`に）。ドキュメントの残りの部分は、構成されています
YAMLはドキュメントがどのように指定する属性を持つ単一のファイル
接続されています。基本的には、マニュアルのように表示することができます二重にリンクされている
を指して、各ドキュメントとドキュメントのリストを前と次の
ドキュメント。内容文書の表は、完全な構造を示しています。

YAMLは次のようなドキュメントを見ての属性：

    ---
    layout: doc_en
    title: How-To - Write Documentation
    previous: Write a Blog Post
    previous_url: how-to/write-a-blog-post
    next: Translate Documentation
    next_url: how-to/translate-documentation
    ---

_layout_の書式設定時に使用するジキルのレイアウトを指定する
ドキュメント。 _layout_は`doc_LANGする必要があります、_LANG_は、ISO- 639- 2です
言語のコード。

_title_の上部に表示されている文書のタイトルを指定します。
ページを表示します。

_previous_と_previous\_url_属性は、タイトルとリンクを与える
以前のドキュメント。同様に、_next_と_next\_url_属性を与える
タイトルと、次のドキュメントのリンクをクリックします。これらは、閲覧を強化するために使用されている
ドキュメントや作業量を再注文部品の必要な制限
マニュアルを参照してください


### 編集既存のドキュメントを

ドキュメントの最初のアウトラインが作成されています。たくさんいる
単に必要なトピックでは、それらのために書かれたドキュメントを持っている。

既存のトピックのドキュメントを追加する、または既存のドキュメントを修正する
`web/doc/LANG`は下のトピックのファイルを開いて、追加または改善
マニュアルを参照してください


### 新しいドキュメントを追加する

既存のトピックでは、存在するのドキュメントを追加するには：

1. Create a new file with the .markdown extension under `web/doc/LANG`.
1. Set up the attributes to link the new file into the existing files. This
   will require editing the _previous_ and _next_ attributes of the existing
   files to insert the new file, as well as adding an entry to
   `index.markdown`.
1. To view your updates while you are working on them, run
   `rbx -S jekyll serve --watch`
1. Edit the new file using Markdown syntax.
1. In the `web/` directory, run `rbx -S jekyll build`.
1. Commit all the changes in the `web/` directory.
