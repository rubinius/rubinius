---
layout: doc_ja
title: Compiler Transforms
previous: Compiler
previous_url: bytecode-compiler/compiler
next: Generator
next_url: bytecode-compiler/generator
translated: true
---

バイトコンパイラは、単純なASTは認識メカニズムを変換した
特定のASTのフォームは、それらを置き換えます。交換形式が異なる発光
元のフォームよりバイトコードが放出される必要があります。のソース
変換は、ファイルは`lib/compiler/ast/transforms.rb`です。

TODO: document the compiler plugin architecture.


### Safe Math Compiler Transform

コアライブラリは、他のRubyのコードと同じブロックのビルドされているため
とRubyはオープンクラスと遅延バインディングと動的言語であるので、それは
違反の方法でFixnumかのような基本的なクラスを変更することが可能
意味は、他のクラスに依存していることを確認します。たとえば、私たちがやったと想像
次：

  class Fixnum
    def +(other)
      (self + other) % 5
    end
  end

それはプラス固定小数点演算を再定義することは確かに可能ですが、
確かに配列のようにいくつかのクラスの原因になりますので、やってモジュロ5は、することができないと
それはする必要があるときに適切な長さを計算します。 Rubyの動的な性質は、
その大切な特徴の一つが、それも本当に両刃の剣である
いくつかの点。

In the standard library the 'mathn' library redefines Fixnum#/ in an unsafe
and incompatible manner. The library aliases Fixnum#/ to Fixnum#quo, which
returns a Float by default.

Because of this there is a special compiler plugin that emits a different
method name when it encounters the #/ method. The compiler emits #divide
instead of #/. The numeric classes Fixnum, Bignum, Float, and Numeric all
define this method.

安全な数学は、コアのコンパイル時に有効になっている変換
ライブラリは、プラグインを有効にします。通常の'user code'をコンパイル中に、
有効になっていないプラグインです。これは、壊すことなく、mathnをサポートすることを可能に
コアライブラリや不便な慣行を余儀なくされた。
