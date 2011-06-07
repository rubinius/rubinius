---
layout: doc_ja
next: Getting Started
next_url: getting-started
translated: true
---

## 何がRubiniusのです

Rubiniusは[Rubyプログラミングの実装です。 言語](http://ruby-lang.org)。

Rubiniusは、バイトコード仮想マシンを、Rubyの文法パーサーが含まれバイトコード
コンパイラは、世代別ガベージコレクタ、ジャストインタイム（JIT）ネイティブのマシン
コードコンパイラ、Rubyのコアおよび標準ライブラリ。

Rubiniusは現在、Rubyのバージョン1.8.7を実装します。


## ライセンス

RubiniusはBSDライセンスを使用しています。ソース内のLICENSEファイルを参照してください。


## インストール

RubiniusはMac OS Xと多くのUnix/
Linuxオペレーティングシステム上で実行されます。サポートするための Microsoft
Windowsは、すぐに来ている。

Rubiniusのをインストールするには、次の手順を実行します。詳細については、
[はじめに](/doc/ja/getting-started/)を参照してください。

1. `git clone git://github.com/rubinius/rubinius.git`
1. `cd rubinius`
1. `./configure --prefix=/path/to/install/dir`
1. `rake install`

インストールプロセスが完了したら、Rubiniusのを追加して指示に従ってくださいする
PATHに実行可能ファイル（bin）にディレクトリを指定します。

RubiniusはRubyGemsのとビルトインとしたすくいとrdocの宝石を付属
プリインストールされています。エゾノコギリソウ宝石をインストールするには、たとえば、インストール`rbx gem install nokogiri`。
