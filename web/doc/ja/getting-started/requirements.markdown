---
layout: doc_ja
title: 必要要件
previous: はじめに
previous_url: getting-started
next: Building
next_url: getting-started/building
---

以下に示すプログラムおよびライブラリがインストールされていることを確認してください。
また、特定のオペレーティングシステムに対する特別な要件については、下のサブセクションを参照してください

後ろに書かれているものは、必要となるプログラムやライブラリに関する提言です。
ご使用のオペレーティングシステムまたはパッケージマネージャによっては、別のパッケージを利用できるかもしれません。

  * [GCC and G++ 4.x](http://gcc.gnu.org/)
  * [GNU Bison](http://www.gnu.org/software/bison/)
  * [MRI Ruby 1.8.7+](http://www.ruby-lang.org/)
    システム上に Ruby 1.8.7 がインストールされていないのならば、[RVM](https://rvm.beginrescueend.com/)
    を使ってインストールすることをおすすめします。
  * [Rubygems](http://www.rubygems.org/)
  * [Git](http://git.or.cz/)
  * [ZLib](http://www.zlib.net/)
  * pthread - The pthread library should be installed by your operating system
  * [gmake](http://savannah.gnu.org/projects/make/)
  * [rake](http://rake.rubyforge.org/) `[sudo] gem install rake`


### Apple OS X

Apple の OS X 上にビルド環境を構築する最も簡単な方法は、XCode Tools と 
Utilities をインストールすることです。
インストールすると、/Developer/Applications/Utilities/CrashReporterPrefs.app の 
developer mode crash reporting を有効にできます。


### Debian/Ubuntu

  * ruby-dev or ruby1.8-dev
  * libreadline5-dev
  * zlib1g-dev
  * libssl-dev


### FreeBSD

Rubinius は、FreeBSD の ports ツリーに lang/rubinius という port をもっています。
この port に関する情報は [http://freshports.org](http://freshports) で得ることができます。
インストールされると、このポートは、全ての依存するものを自動的にインストールします。
