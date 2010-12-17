---
layout: doc_ja
title: Profiler
previous: Debugger
previous_url: tools/debugger
next: Memory Analysis
next_url: tools/memory-analysis
translated: true
---

Rubiniusはすべての正確なタイミングを提供していますインストルメントプロファイラを持って
メソッドは、その実行されます。プロファイラは、VMレベルのデータで実装されています
プロセスにRubyのコードに提供されています。


## VMのプロファイラは、

作成および維持するプロファイラに携わった文字のキャスト
VMは、SharedState、ProfilerCollectionと、プロファイラが含まれています。 VMのクラスは
スレッドローカルデータ構造。各VMのインスタンスは、個別のプロファイラを取得
インスタンスを作成します。 SharedStateインスタンスは、そのマップProfilerCollectionのインスタンスを持つ
プロファイラにVMは、すべてのプロファイラの複合結果を生成します。

プロファイラが住んでおり、独自の世界で死ぬ。プロファイラは、VMを渡される
プロファイラは、それへのアクセスを必要とするため、それが作成されるインスタンス時間
情報を収集しています。国の引数は、すべてのプロファイラに渡すことができます
メソッドが、それは十分なプロファイラが作成されたときにそれを渡すことは簡単です。
プロファイラは決してVMインスタンスを操作します。これは、維持することが重要です
この分離。

VMのインスタンスは、遅延、必要に応じて、そのプロファイラのインスタンスが作成されます。 VMの
SharedStateとプロファイラを登録します。

SharedStateはProfilerCollectionのインスタンスを保持転送コール
登録したり、プロファイラを削除してください。

VMインスタンスにマップされたProfilerCollectionインスタンスを要求
プロファイラは、プロファイラは削除されますプロファイラを削除します。


## Rubyのプロファイラを

Rubyの土地では、Rubiniusは：：プロファイラ：：計測機能のインスタンスが何を公開
VMでマルチスレッドの現実について。個々のC + +のプロファイラ
インスタンスは、Rubyには公開されません。 Rubyでは、プロファイラのインスタンスが作成されます。
そのインスタンスは、単に起動することができますおよびVMプロファイラを停止します。ときにプロファイラは、
停止すると、プロファイリングに関する情報は、LookupTableのが返されます。ルビー
プロファイラコードは、情報が＃showメソッドを使用して表示することができます。

Rubiniusは互換性のある標準ライブラリprofile.rbとprofiler.rbを提供しています。
参照してくださいMRIにそれらを使用する方法についてはマニュアルを参照してください

あなたはプロファイラを使用するための基本的な手順を参照してください`lib/profiler.rb`のですることができますみる
Rubyのコードから。

    # create a profiler instance
    profiler = Rubinius::Profiler::Instrumenter.new

    # start the profiler
    profiler.start

    # stop the profiler
    profiler.stop

    # get the profile data
    data = profiler.info

    # or print out the profiler info
    profiler.show  # takes on IO object, defaults to STDOUT

また、ブロック内の作業をプロファイリングするための便利なメソッドを使用することができます。

    # create a profiler instance
    profiler = Rubinius::Profiler::Instrumenter.new

    # profile some work
    profiler.profile do
      # some work
    end

\#profileはメソッドは、プロファイラを、利回り起動し、プロファイラを停止し、出力
デフォルトではプロファイルデータ。パスは、'false'にデータを印刷しないように#profileする。
どちらにしても、プロファイルデータ自体は#profileによって返されます。


## How to Read the Flat Profiler Output

The flat profiler output has the following columns:


### % time

The amount of time spent in this method as a percentage of the total time
spent in all methods.


### cumulative seconds

The total amount of time spent in this method and all its direct callees and
their callees all the way to every leaf method called along a path from this
method. Consider this method as the root of a call tree. The sum of all the
time spent in methods in this call tree is the cumulative seconds for this
method.


### self seconds

The total time spent in this method less the total time spent in all this
method's callees.


### calls

The total number of times this method was called.


### self ms/call

The self seconds as milliseconds divided by the total number of calls.


### total ms/call

The cumulative seconds as milliseconds divided by the total number of calls.


### Example of Flat Output

The following script is the basis of both profile examples below.

    class F
      def foo(a)
        1 + a
      end

      def bar(a, b)
        foo(a) ** b
      end

      def work(a, b, n, m)
        n.times { |i| i + bar(a, b) }
        m.times { |i| foo(i) }
      end
    end

    p = Rubinius::Profiler::Instrumenter.new :sort => :self_seconds
    p.profile {
      F.new.work 2, 3, 10, 5
    }


Running the script with 'bin/rbx script.rb' should give the following flat
output.


      %   cumulative   self                self     total
     time   seconds   seconds      calls  ms/call  ms/call  name
    ------------------------------------------------------------
      9.64     0.00      0.00         10     0.00     0.00  Fixnum#**
     23.33     0.00      0.00          1     0.01     0.08  #toplevel
     12.64     0.00      0.00         10     0.00     0.00  F#bar
     15.38     0.00      0.00         15     0.00     0.00  F#work {}
     17.74     0.00      0.00          2     0.00     0.03  Integer#times
     19.29     0.00      0.00          1     0.01     0.06  F#work
      1.12     0.00      0.00          1     0.00     0.00  Class#new
      0.68     0.00      0.00         15     0.00     0.00  F#foo
      0.13     0.00      0.00          1     0.00     0.00  Class#allocate
      0.06     0.00      0.00          1     0.00     0.00  Object#initialize

    10 methods called a total of 57 times


How to Read the Graph Output
----------------------------

The graph output is enabled with the configuration option:

    -Xprofiler.graph

Given the same script above, the graph output is shown below. Each "entry" in
the graph has three sections: 1) the method for the entry, called the
_primary_ line; 2) the callers of the primary method; and 3) the methods that
the primary method called. The fields have different meanings based on the
part of the entry.

For the primary line, the fields are as follows:


### index

An index assigned to each method in the graph to facilitate cross-referencing
the entries.


### % time

The amount of time spent in this method as a percentage of the total time
spent in all methods. This is the same as the flat output.


### self

The total time spent in this method less the total time spent in all this
method's callees. This is the same as self seconds in the flat output.


### children

The total time spent in all the methods called by this method.


### called

The total number of times this method was called.


### name

The name of the method followed by the index number.


The lines above the primary line are methods that call the primary method. The
callers' fields have the following interpretation:


### self

The total time spent in this method less the total time spent in all this
method's callees. This is the same as self seconds in the flat output.


### children

The time spent in the method's call to the primary method.


### called

The called field has two parts separated by a forward slash. The left is the
number of times this method called the primary method. The right is the total
number of calls this method made. In other words, the two numbers together
show a ratio of the calls to the primary method versus all calls made by the
caller.


### name

The name of the caller followed by its index number. If the index is [0], the
method does not appear in the graph.


The lines below the primary line are methods that the primary method called.
The fields for the called methods are as follows:


### self

The total time spent in this method less the total time spent in all this
method's callees. This is the same as self seconds in the flat output.


### children

This is an estimate of the amount of time this method's callees spent when
this method was called by the primary method. The estimate is based on the
ration of the time this method spent when called by the primary method to the
total time spent in this method.


### called

The called field has two parts separated by a forward slash. The left is the
number of times this method was called by the primary method. The right is the
total number of times this method was called.


### name

The name of the called method followed by its index number [N]. If there is no
index present, there is no primary entry for the method in the graph. Use the
-Xprofiler.full_report option to print the entire graph if you need to view
the entry.


    index  % time     self  children         called       name
    ----------------------------------------------------------
                      0.00      0.00         10/20             F#bar [3]
    [1]       9.6     0.00      0.00         10           Fixnum#** [1]
    -------------------------------------------------------
    [2]      23.2     0.00      0.00          1           #toplevel [2]
                      0.00      0.00          1/1              Class#new [7]
                      0.00      0.00          1/1              F#work [6]
    -------------------------------------------------------
                      0.00      0.00         10/15             F#work {} [4]
    [3]      12.7     0.00      0.00         10           F#bar [3]
                      0.00      0.00         10/15             F#foo [8]
                      0.00      0.00         10/10             Fixnum#** [1]
    -------------------------------------------------------
                      0.00      0.00         15/15             Integer#times [5]
    [4]      15.4     0.00      0.00         15           F#work {} [4]
                      0.00      0.00         10/10             F#bar [3]
                      0.00      0.00          5/15             F#foo [8]
    -------------------------------------------------------
                      0.00      0.00          2/2              F#work [6]
    [5]      17.8     0.00      0.00          2           Integer#times [5]
                      0.00      0.00         15/15             F#work {} [4]
    -------------------------------------------------------
                      0.00      0.00          1/2              #toplevel [2]
    [6]      19.3     0.00      0.00          1           F#work [6]
                      0.00      0.00          2/2              Integer#times [5]
    -------------------------------------------------------
                      0.00      0.00          1/2              #toplevel [2]
    [7]       1.1     0.00      0.00          1           Class#new [7]
                      0.00      0.00          1/1              Object#initialize [10]
                      0.00      0.00          1/1              Class#allocate [9]
    -------------------------------------------------------
                      0.00      0.00         10/20             F#bar [3]
                      0.00      0.00          5/15             F#work {} [4]
    [8]       0.7     0.00      0.00         15           F#foo [8]
    -------------------------------------------------------
                      0.00      0.00          1/2              Class#new [7]
    [9]       0.1     0.00      0.00          1           Class#allocate [9]
    -------------------------------------------------------
                      0.00      0.00          1/2              Class#new [7]
    [10]      0.1     0.00      0.00          1           Object#initialize [10]
    -------------------------------------------------------

    10 methods called a total of 57 times

