---
layout: doc_ja
title: Ruby Parser
previous: Bytecode Compiler
previous_url: bytecode-compiler
next: AST
next_url: bytecode-compiler/ast
review: true
translated: true
---

パーサーは、メルボルンという名前のCの拡張機能です。これは基本的にパーサから
MRIは、Rubiniusのに使用することができるように包まれた。パーサはRubyを変換する
ノードの内部パースツリーのソースコード。構文解析ツリーによって処理される
各ノードのRubyのメソッドを呼び出す。プロセッサがある
libに/メルボルン/ processor.rb。プロセッサのメソッドは、Rubyのオブジェクトを作成する
それは抽象構文木（AST）の内のノードです。 ASTは処理されて
バイトコードを生成します。
