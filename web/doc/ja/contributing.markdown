---
layout: doc_ja
title: Contributing
previous: Troubleshooting
previous_url: getting-started/troubleshooting
next: Communication
next_url: contributing/communication
translated: true
---

Rubiniusプロジェクトは、あなたの貢献を歓迎しています。には多くのものがあります。
手伝って。最も重要なのは、あなたが何かを行う必要があることの利益。
それは情熱とエネルギーをあなたの仕事を持っているための最良の方法です。

あなたはRubiniusの疑問がある場合は、答えを得るために最善の方法は、チャットすることです
irc.freenode.netの＃RubiniusのIRCチャンネルで私たちと。

以下は、物事がRubiniusの上で行うためのいくつかのアイデアです。


## 自分のコードを実行する

あなたのコードは多くの場合、仕様よりも悪質です。実行してあなたのペットのプロジェクトの下に
Rubiniusの、レポートの問題。見る[チケットお書きくださいする方法]を（/ docに/ jaを/ハウツー/書き込み-チケット）。


## 助けを求める

私達は助けることに何かできることが、我々がします。独自の研究を行うことを確認してください
も、可能であれば。 Rubiniusは、簡単に学ぶことができますプロジェクトにしようとすると
から学び、できれば拡張します。

我々は確かに受け入れて、単純なバグ報告をお待ちしておりますが、私たちは与えることができる
問題を再現するための簡単な手順が含まれてチケットを優先度を設定します。も
より良いバグを示すRubySpecsおよびパッチを含むチケットです
それはそれを修正しています。


## Write Specs

  1. Run `bin/mspec tag --list incomplete <dir>` to show specs that have been
     tagged as incomplete. These specs may simply need review, or there could
     be specs missing for a particular class.

     NOTE: You can specify the pseudo-directory ':files' for \<dir\>, which will
     show tags for all the specs that should run on Rubinius. Or you can
     specify any subdirectory of the spec/ directory to list tags for specs in
     that subdirectory.

  2. Find unspecified behaviors. See [How To Write a Ruby
     Spec](/doc/ja/how-to/write-a-ruby-spec).


## Fix Failing Specs

  1. Run `bin/mspec tag --list fails <dir>` to show specs tagged as failing.

     NOTE: You can specify the pseudo-directory ':files' for \<dir\>, which will
     show tags for all the specs that should run on Rubinius. Or you can
     specify any subdirectory of the spec/ directory to list tags for specs in
     that subdirectory.

  2. Pick a spec that looks interesting and see if you can write a patch that
     will make it pass.


## Write Docs

Study how Rubinius works and write highlevel documentation that will help
others learn about the implementation details.

## Triage Tickets

  * Revive or close old tickets.
  * Build minimal test cases that reproduce the bugs. Then see if there are
    already RubySpecs for the issue. If not, consider writing some.
