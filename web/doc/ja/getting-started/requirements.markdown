---
layout: doc_ja
title: Requirements
previous: Getting Started
previous_url: getting-started
next: Building
next_url: getting-started/building
translated: true
---

を確認するには、次のプログラムおよびインストールされているライブラリを持っています。また、参照してください
特定のオペレーティングシステム用の特別な要件については、以下のサブセクション
システムです。

次のプログラムに関する詳細情報を取得するための提案です
とライブラリがRubiniusのを構築する必要がありました。ご使用のオペレーティングシステムまたはパッケージ
マネージャは、他のパッケージが利用できる場合があります。

  * [GCC and G++ 4.x](http://gcc.gnu.org/)
  * [GNU Bison](http://www.gnu.org/software/bison/)
  * [MRI Ruby 1.8.7+](http://www.ruby-lang.org/) If your system does not have
    Ruby 1.8.7 installed, consider using [RVM](https://rvm.beginrescueend.com/)
    to install it.
  * [Rubygems](http://www.rubygems.org/)
  * [Git](http://git.or.cz/)
  * [ZLib](http://www.zlib.net/)
  * pthread - The pthread library should be installed by your operating system
  * [gmake](http://savannah.gnu.org/projects/make/)
  * [rake](http://rake.rubyforge.org/) `[sudo] gem install rake`


### Apple OS X

アップルのOS X上でビルド環境を取得する最も簡単な方法は、インストールすることです。
Xcodeのツールとユーティリティ。インストールが完了すると、あなたは開発者モードの
クラッシュを有効にすることができますでのレポート:
/Developer/Applications/Utilities/CrashReporterPrefs.app


### Debian/Ubuntu

  * ruby-dev or ruby1.8-dev
  * libreadline5-dev
  * zlib1g-dev
  * libssl-dev
