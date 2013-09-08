---
layout: doc_ja
title: How-To - Write Documentation
previous: Write a Blog Post
previous_url: how-to/write-a-blog-post
next: Translate Documentation
next_url: how-to/translate-documentation
translated: true
---

Rubiniusのドキュメントは、Webサイトとブログで構成されています。他の部分と同じくJekyllを使用しています。

始めに、`kramdown`と`jekyll`がインストールされていることを確かめてください。

    rbx gem install jekyll kramdown

ドキュメントは、`web/doc`のディレクトリにあります。
各言語のサブディレクトリには、翻訳されたドキュメントががあります。（例えば、`en`、`es`など）

翻訳毎に目次があります。（例えば、`/web/doc/ja/index.markdown`）。
ドキュメントの残りの部分は、どのように他のドキュメントとリンクが張ってあるかというYAMLの属性から成る単一のファイルです。
基本的に、ドキュメントは前後のドキュメントにリンクが張ってある双方向リストとみなせます。

ドキュメントのYAMLの属性は下記の通りです。

    ---
    layout: doc_en
    title: How-To - Write Documentation
    previous: Write a Blog Post
    previous_url: how-to/write-a-blog-post
    next: Translate Documentation
    next_url: how-to/translate-documentation
    ---

_layout_は、どのレイアウトをjekyllが使用するか指定します。
_layout_の_LANG_は、ISO-639-2の`doc_LANGにしなければいけません。

_title_は、ページの上部に表示されるドキュメントのタイトルを指定します。

_previous_と_previous\_url_属性で、前のドキュメントのタイトルとリンクを指定します。
同様に、_next_と_next\_url_属性で、後のドキュメントのタイトルとリンクを指定します。

これらはドキュメントの閲覧を便利にする、再度ドキュメントを書くときに必要な作業量を削減するためです。

RubiniusのリポジトリにJekyllで生成したドキュメントとファイルがコミットされます。リポジトリをクローンしたら、Rubiniusをインストールする前かRubiniusのビルドに問題があるとき
`rake docs`でドキュメントを閲覧することができます。

### 既存ドキュメントの編集

ドキュメントの最初のアウトラインが作成されています。
ドキュメントを書く必要がある話題がたくさんあります。

既に存在している話題にドキュメントを追加するかドキュメントを修正する場合:

1. `web/doc/LANG`の下のファイルを開く。
1. ドキュメントの追加または改善を行う。
1. 作業中のドキュメントのアップデートを見る。
  `web/`ディレクトリで`rbx -S jekyll serve --watch`を実行する。
1. ドキュメントの追加または編集が終了したら、ソースコードの変更をコミットする。
1. `web/`ディレクトリで`rbx -S jekyll build`を実行し、`web/_site`の全てのファイルを強制的にアップデートする。
1. 生成したファイルをコミットする。変更が少ない場合、ドキュメントのソースコードと生成したファイルをコミットしてください。
   変更が多い場合、レビューでの変更が単純になるように生成したファイルを個別にコミットしてください。

### 新しいドキュメントの追加

存在してない話題をのドキュメントを追加する場合：

1. `web/doc/LANG`に.markdown拡張子のファイルを作成する。
1. 既に存在してるファイルから新しいファイルにリンクを張るために属性を記述する。
`index.markdown`と既に存在しているファイルの_previous_ と _next_ attributesに新しいファイルへのリンクを記述することが必要です。
1. 作業中にアップデートしたドキュメントを見るには、`rbx -S jekyll serve --watch`を実行します。
1. マークダウン記法で新しいファイルを編集します。
1. ドキュメントの追加または編集が終わったら、変更をコミットしてください。
1. `rbx -S jekyll build` in the `web/` を実行し、`web/_site`で生成されているファイルを強制的にアップデートしてください。
1. 生成したファイルをコミットする。変更が少ない場合、ドキュメントのソースコードと生成したファイルをコミットしてください。
   変更が多い場合、レビューでの変更が単純になるように生成したファイルを個別にコミットしてください。