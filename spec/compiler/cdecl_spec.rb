require File.dirname(__FILE__) + '/../spec_helper'

describe "A Cdecl node" do
  relates "X = 42" do
    parse do
      [:cdecl, :X, [:lit, 42]]
    end

    compile do |g|
      g.push_scope
      g.push_literal :X
      g.push 42
      g.send :const_set, 2
    end
  end

  relates "::X = 1" do
    parse do
      [:cdecl, [:colon3, :X], [:lit, 1]]
    end

    compile do |g|
      g.push_cpath_top
      g.push_literal :X
      g.push 1
      g.send :const_set, 2
    end
  end

  relates "X::Y = 1" do
    parse do
      [:cdecl, [:colon2, [:const, :X], :Y], [:lit, 1]]
    end

    compile do |g|
      g.push_const :X
      g.push_literal :Y
      g.push 1
      g.send :const_set, 2
    end
  end

  relates "X::Y::Z = a" do
    parse do
      [:cdecl,
       [:colon2, [:colon2, [:const, :X], :Y], :Z],
       [:call, nil, :a, [:arglist]]]
    end

    compile do |g|
      g.push_const :X
      g.find_const :Y
      g.push_literal :Z
      g.push :self
      g.send :a, 0, true
      g.send :const_set, 2
    end
  end

  relates "a::A = 1" do
    parse do
      [:cdecl, [:colon2, [:call, nil, :a, [:arglist]], :A], [:lit, 1]]
    end

    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.push_literal :A
      g.push 1
      g.send :const_set, 2
    end
  end

  relates <<-ruby do
      a = Object
      a::B = b
    ruby

    parse do
      [:block,
       [:lasgn, :a, [:const, :Object]],
       [:cdecl, [:colon2, [:lvar, :a], :B], [:call, nil, :b, [:arglist]]]]
    end

    compile do |g|
      g.push_const :Object
      g.set_local 0
      g.pop
      g.push_local 0
      g.push_literal :B
      g.push :self
      g.send :b, 0, true
      g.send :const_set, 2
    end
  end
end
