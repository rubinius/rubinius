require File.dirname(__FILE__) + '/../spec_helper'

describe "A Masgn node" do
  relates "a, b = c, d" do
    parse do
      [:masgn,
       [:array, [:lasgn, :a], [:lasgn, :b]],
       [:array, [:call, nil, :c, [:arglist]], [:call, nil, :d, [:arglist]]]]
    end

    # masgn
  end

  relates "a, b, *c = 1, 2, *[3, 4]" do
    parse do
      [:masgn,
       [:array, [:lasgn, :a], [:lasgn, :b], [:splat, [:lasgn, :c]]],
       [:array, [:lit, 1], [:lit, 2], [:splat, [:array, [:lit, 3], [:lit, 4]]]]]
    end

    # masgn argscat
  end

  relates "a.b, a.c, _ = q" do
    parse do
      [:masgn,
       [:array,
        [:attrasgn, [:call, nil, :a, [:arglist]], :b=, [:arglist]],
        [:attrasgn, [:call, nil, :a, [:arglist]], :c=, [:arglist]],
        [:lasgn, :_]],
       [:to_ary, [:call, nil, :q, [:arglist]]]]
    end

    # masgn attrasgn array rhs
  end

  relates <<-ruby do
      a, i, j = [], 1, 2
      a[i], a[j] = a[j], a[i]
    ruby

    parse do
      [:block,
       [:masgn,
        [:array, [:lasgn, :a], [:lasgn, :i], [:lasgn, :j]],
        [:array, [:array], [:lit, 1], [:lit, 2]]],
       [:masgn,
        [:array,
         [:attrasgn, [:lvar, :a], :[]=, [:arglist, [:lvar, :i]]],
         [:attrasgn, [:lvar, :a], :[]=, [:arglist, [:lvar, :j]]]],
        [:array,
         [:call, [:lvar, :a], :[], [:arglist, [:lvar, :j]]],
         [:call, [:lvar, :a], :[], [:arglist, [:lvar, :i]]]]]]
    end

    # masgn attrasgn idx
  end

  relates "a, b.c = d, e" do
    parse do
      [:masgn,
       [:array,
        [:lasgn, :a],
        [:attrasgn, [:call, nil, :b, [:arglist]], :c=, [:arglist]]],
       [:array, [:call, nil, :d, [:arglist]], [:call, nil, :e, [:arglist]]]]
    end

    # masgn attrasgn
  end

  relates "*a.m = *b" do
    parse do
      [:masgn,
       [:array,
        [:splat, [:attrasgn, [:call, nil, :a, [:arglist]], :m=, [:arglist]]]],
       [:splat, [:call, nil, :b, [:arglist]]]]
    end

    # masgn attrasgn splat lhs splat rhs
  end

  relates "*a.m = b" do
    parse do
      [:masgn,
       [:array,
        [:splat, [:attrasgn, [:call, nil, :a, [:arglist]], :m=, [:arglist]]]],
       [:to_ary, [:call, nil, :b, [:arglist]]]]
    end

    # masgn attrasgn splat lhs
  end

  relates "A, B, C = 1, 2, 3" do
    parse do
      [:masgn,
       [:array, [:cdecl, :A], [:cdecl, :B], [:cdecl, :C]],
       [:array, [:lit, 1], [:lit, 2], [:lit, 3]]]
    end

    # masgn cdecl
  end

  relates "* = 1, 2" do
    parse do
      [:masgn, [:array, [:splat]], [:array, [:lit, 1], [:lit, 2]]]
    end

    # masgn empty lhs splat
  end

  relates "*$a = b" do
    parse do
      [:masgn,
       [:array, [:splat, [:gasgn, :$a]]],
       [:to_ary, [:call, nil, :b, [:arglist]]]]
    end

    # masgn gasgn splat lhs
  end

  relates "*$a = *b" do
    parse do
      [:masgn,
       [:array, [:splat, [:gasgn, :$a]]],
       [:splat, [:call, nil, :b, [:arglist]]]]
    end

    # masgn gasgn splat lhs splat rhs
  end

  relates "a, @b = c, d" do
    parse do
      [:masgn,
       [:array, [:lasgn, :a], [:iasgn, :@b]],
       [:array, [:call, nil, :c, [:arglist]], [:call, nil, :d, [:arglist]]]]
    end

    # masgn iasgn
  end

  relates "*@a = b" do
    parse do
      [:masgn,
       [:array, [:splat, [:iasgn, :@a]]],
       [:to_ary, [:call, nil, :b, [:arglist]]]]
    end

    # masgn iasgn splat lhs
  end

  relates "*@a = *b" do
    parse do
      [:masgn,
       [:array, [:splat, [:iasgn, :@a]]],
       [:splat, [:call, nil, :b, [:arglist]]]]
    end

    # masgn iasgn splat lhs splat rhs
  end

  relates "*a = 1, 2, 3" do
    parse do
      [:masgn,
       [:array, [:splat, [:lasgn, :a]]],
       [:array, [:lit, 1], [:lit, 2], [:lit, 3]]]
    end

    # masgn lasgn splat lhs array
  end

  relates "*a = b" do
    parse do
      [:masgn,
       [:array, [:splat, [:lasgn, :a]]],
       [:to_ary, [:call, nil, :b, [:arglist]]]]
    end

    # masgn lasgn splat lhs
  end

  relates "*a = *b" do
    parse do
      [:masgn,
       [:array, [:splat, [:lasgn, :a]]],
       [:splat, [:call, nil, :b, [:arglist]]]]
    end

    # masgn lasgn splat lhs splat rhs
  end

  relates "a, (b, c) = [1, [2, 3]]" do
    parse do
      [:masgn,
       [:array, [:lasgn, :a], [:masgn, [:array, [:lasgn, :b], [:lasgn, :c]]]],
       [:to_ary, [:array, [:lit, 1], [:array, [:lit, 2], [:lit, 3]]]]]
    end

    # masgn masgn
  end

  relates "a, = *[[[1]]]" do
    parse do
      [:masgn,
       [:array, [:lasgn, :a]],
       [:splat, [:array, [:array, [:array, [:lit, 1]]]]]]
    end

    # masgn splat no name array
  end

  relates "a, b, * = c" do
    parse do
      [:masgn,
       [:array, [:lasgn, :a], [:lasgn, :b], [:splat]],
       [:to_ary, [:call, nil, :c, [:arglist]]]]
    end

    # masgn splat no name to ary
  end

  relates "a, b, = c" do
    parse do
      [:masgn,
       [:array, [:lasgn, :a], [:lasgn, :b]],
       [:to_ary, [:call, nil, :c, [:arglist]]]]
    end

    # masgn splat no name trailing
  end

  relates "a, b, *c = d, e, f, g" do

    parse do
      [:masgn,
       [:array, [:lasgn, :a], [:lasgn, :b], [:splat, [:lasgn, :c]]],
       [:array,
        [:call, nil, :d, [:arglist]],
        [:call, nil, :e, [:arglist]],
        [:call, nil, :f, [:arglist]],
        [:call, nil, :g, [:arglist]]]]
    end

    # masgn splat
  end

  relates "a, b, *c = d.e(\"f\")" do
    parse do
      [:masgn,
       [:array, [:lasgn, :a], [:lasgn, :b], [:splat, [:lasgn, :c]]],
       [:to_ary, [:call, [:call, nil, :d, [:arglist]], :e, [:arglist, [:str, "f"]]]]]
    end

    # masgn splat to ary2
  end

  relates "a, b, *c = d" do
    parse do
      [:masgn,
       [:array, [:lasgn, :a], [:lasgn, :b], [:splat, [:lasgn, :c]]],
       [:to_ary, [:call, nil, :d, [:arglist]]]]
    end

    # masgn splat to ary
  end

  relates "a, b = c" do
    parse do
      [:masgn,
       [:array, [:lasgn, :a], [:lasgn, :b]],
       [:to_ary, [:call, nil, :c, [:arglist]]]]
    end

    # to ary
  end
end
