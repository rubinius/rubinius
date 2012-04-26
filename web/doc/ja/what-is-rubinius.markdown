---
layout: doc_ja
title: Rubinius とは
next: はじめに
next_url: getting-started
---

## Rubinius とは

Rubinius は [プログラミング言語 Ruby](http://ruby-lang.org) の実装です。

Rubinius には、バイトコード仮想マシン、Ruby 
の構文解析器、バイトコードコンパイラ、世代別ガベージコレクタ、ジャストインタイム 
(JIT) ネイティブマシンコードコンパイラ、そして、Ruby 
のコアライブラリと標準ライブラリが含まれています。

現在のところ、Rubinius は Ruby のバージョン 1.8.7 の実装となっています。


## ライセンス

Rubinius は BSD ライセンスを使用しています。
ソース内の LICENSE ファイルを参照してください。


## インストール

Rubinius は、Mac OS X と多くの Unix/Linux オペレーティングシステム上で実行されます。
Microsoft Windows のサポートも間近です。

Rubinius のをインストールするには、次の手順を実行します。
詳細については、[はじめに](/doc/ja/getting-started/) を参照してください。

1. `git clone git://github.com/rubinius/rubinius.git`
1. `cd rubinius`
1. `./configure --prefix=/path/to/install/dir`
1. `rake install`

インストールプロセスが完了した後は、Rubinius の実行ディレクトリ (bin) 
を PATH に追加するための指示に従ってください。

Rubinius には RubyGems が組み込まれており、rake と rdoc の 2 つの gem 
は予めインストールされています。
例として、nokogiri gem をインストールするには `rbx gem install nokogiri` 
を実行してください。
