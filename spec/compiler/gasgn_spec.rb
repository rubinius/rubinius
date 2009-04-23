require File.dirname(__FILE__) + '/../spec_helper'

describe "A Gasgn node" do
  relates "$x = 42" do
    parse do
      [:gasgn, :$x, [:lit, 42]]
    end

    compile do |g|
      g.push_const :Rubinius
      g.find_const :Globals
      g.push_literal :"$x" # REFACTOR g.get_global("$x")
      g.push 42
      g.send :[]=, 2
    end
  end

  relates "$a = *[1]" do
    parse do
      [:gasgn, :$a, [:svalue, [:splat, [:array, [:lit, 1]]]]]
    end

    # gasgn splat
  end
end
