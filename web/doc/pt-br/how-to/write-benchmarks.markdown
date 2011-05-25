---
layout: doc_pt_br
title: How-To - Write Benchmarks
previous: Fix a Failing Spec
previous_url: how-to/fix-a-failing-spec
next: Write a Blog Post
next_url: how-to/write-a-blog-post
---

Why benchmarks?

Benchmarks are a great tool for comparing Rubinius with the rest of the Ruby
runtimes, i.e. MRI, JRuby, IronRuby and other implementations. It is not so
much intended to measure Rubinius itself so if you want to contribute writing
benchmarks make sure to follow these steps:

  1.  Find existing benchmarks in rubinius/benchmarks and study their layout.
  3.  Each benchmark file should measure one specific aspect of Ruby. For 
      example different ways of deleting keys/values from a Hash.
  4.  Use the benchmark framework.
  5.  Keep the benchmarks short and simple.
  6.  The benchmarks are not meant to measure Rubinius. So if you're writing a 
      benchmark for a class with bang and no-bang methods you will want to use
      a duplicate of a variable in the bang method but you don't need to dup
      in the no-bang method. 

If you want to test a benchmark you can run it file or even directory wise:

    bin/benchmark benchmark/core/string/bench_case.rb
    bin/benchmark benchmark/core
