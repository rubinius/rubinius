require File.dirname(__FILE__) + '/../spec_helper'

describe "An Attrasgn node" do
  relates <<-ruby do
      y = 0
      42.method = y
    ruby

    parse do
      [:block,
       [:lasgn, :y, [:lit, 0]],
       [:attrasgn, [:lit, 42], :method=, [:arglist, [:lvar, :y]]]]
    end

    # attrasgn
  end

  relates "a.m = *[1]" do
    parse do
      [:attrasgn,
       [:call, nil, :a, [:arglist]],
       :m=,
       [:arglist, [:svalue, [:splat, [:array, [:lit, 1]]]]]]
    end

    # attrasgn_splat
  end

  relates "a[*b] = c" do
    parse do
      [:attrasgn,
       [:call, nil, :a, [:arglist]],
       :[]=,
       [:arglist,
        [:splat, [:call, nil, :b, [:arglist]]],
        [:call, nil, :c, [:arglist]]]]
    end

    # argspush
  end

  relates "a[42] = 24" do
    parse do
      [:attrasgn,
       [:call, nil, :a, [:arglist]],
       :[]=,
       [:arglist, [:lit, 42], [:lit, 24]]]
    end

    # attrasgn_index
  end

  relates <<-ruby do
      a = []
      a [42] = 24
    ruby

    parse do
      [:block,
       [:lasgn, :a, [:array]],
       [:attrasgn, [:lvar, :a], :[]=, [:arglist, [:lit, 42], [:lit, 24]]]]
    end

    # attrasgn_index_equals
  end
end
