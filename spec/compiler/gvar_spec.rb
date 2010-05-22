require File.dirname(__FILE__) + '/../spec_helper'

describe "A Gvar node" do
  relates "$x" do
    compile do |g|
      g.push_rubinius
      g.find_const :Globals
      g.push_literal :"$x" # REFACTOR g.get_global("$x")
      g.send :[], 1
    end
  end

  relates "$stderr" do
    compile do |g|
      g.push_rubinius
      g.find_const :Globals
      g.push_literal :"$stderr"
      g.send :[], 1
    end
  end

  relates "$__blah" do
    compile do |g|
      g.push_rubinius
      g.find_const :Globals
      g.push_literal :"$__blah"
      g.send :[], 1
    end
  end

  relates "$_" do
    compile do |g|
      g.push_rubinius
      g.find_const :Globals
      g.push_literal :"$_"
      g.send :[], 1
    end
  end
end
