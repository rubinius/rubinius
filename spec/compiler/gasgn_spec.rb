require File.dirname(__FILE__) + '/../spec_helper'

describe "A Gasgn node" do
  relates "$x = 42" do
    compile do |g|
      g.push_rubinius
      g.find_const :Globals
      g.push_literal :"$x" # REFACTOR g.get_global("$x")
      g.push 42
      g.send :[]=, 2
    end
  end

  relates "$a = *[1]" do
    # gasgn splat
  end
end
