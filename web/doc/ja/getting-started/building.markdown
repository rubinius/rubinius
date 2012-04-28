---
layout: doc_ja
title: Building Rubinius
previous: Requirements
previous_url: getting-started/requirements
next: Running Rubinius
next_url: getting-started/running-rubinius
translated: true
---

あなたが構築することができますし、ソースディレクトリからRubiniusの
を実行してください。あなたがする必要はありません。 Rubiniusは
それを実行するためにインストールしてください。方向は、以下の両方
のインストールについて詳しく説明しますRubiniusの、ソースディレクトリ
から、それを実行している。

RubiniusはJITコンパイラのLLVMのを使用しています。 Rubiniusは、特定の依存
LLVMのバージョンの。お使いのシステムにLLVMのインストールされている場合、パス
`--skip-system`フラグは、以下の方向でスクリプトを設定してください。


### Getting the Source

RubiniusのソースコードはtarballとしてのGitHubにプロジェクトとして利用可能です。
あなたは[ここのtarballをダウンロード](https://github.com/rubinius/rubinius/tarball/master)することができます。

To use Git:

  1. Change your current working directory to your development directory
  2. `git clone git://github.com/rubinius/rubinius.git`


### Installing Rubinius

あなたのアプリケーションを実行するためにRubiniusの利用を計画している場合、これは良いです
オプションを選択します。ただし、また、ソースディレクトリから直接Rubiniusのを実行することができます。
ことの詳細については、次のセクションを参照してください。

我々は、`必要としないや`sudo場所にRubiniusのをインストールすることをお勧め
スーパーユーザー権限が必要です。 Rubiniusのをインストールするには：

  1. `./configure --prefix=/path/to/install/dir`
  2. `rake install`
  3. Follow the directions to add the Rubinius _bin_ directory to your PATH


### Running from the Source Directory

If you plan to work on Rubinius itself, you should use this option.

  1. `./configure`
  2. `rake`

あなただけのRubiniusのをしようとしている場合は、_bin_を追加して指示に従ってください
PATHにディレクトリを指定します。

ただし、Rubiniusの開発を行っている場合は、NOTは_bin_を追加する必要があります。
Rubiniusはシステムを構築するため、PATHにディレクトリをピックアップする
`ruby`のと`熊手は`Rubiniusの実行可能ファイルへのリンクを示します。 Rubiniusは別のを必要とする
Rubyは実行可能なプロセスを構築する時に自分自身をブートストラップする。
