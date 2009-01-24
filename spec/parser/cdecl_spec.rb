require File.dirname(__FILE__) + '/../spec_helper'

describe "A Cdecl node" do
  relates "X = 42" do
    parse do
      [:cdecl, :X, [:lit, 42]]
    end

    compile do |g|
      g.push_context
      g.push_literal :X
      g.push 42
      g.send :__const_set__, 2
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
      g.send :__const_set__, 2
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
      g.send :__const_set__, 2
    end
  end
end
