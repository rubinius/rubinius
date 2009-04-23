require File.dirname(__FILE__) + '/../spec_helper'

describe "A Gvar node" do
  relates "$x" do
    parse do
      [:gvar, :$x]
    end

    compile do |g|
      g.push_const :Rubinius
      g.find_const :Globals
      g.push_literal :"$x" # REFACTOR g.get_global("$x")
      g.send :[], 1
    end
  end

  relates "$stderr" do
    parse do
      [:gvar, :$stderr]
    end

    compile do |g|
      g.push_const :Rubinius
      g.find_const :Globals
      g.push_literal :"$stderr"
      g.send :[], 1
    end
  end

  relates "$__blah" do
    parse do
      [:gvar, :$__blah]
    end

    compile do |g|
      g.push_const :Rubinius
      g.find_const :Globals
      g.push_literal :"$__blah"
      g.send :[], 1
    end
  end

  relates "$_" do
    parse do
      [:gvar, :$_]
    end

    compile do |g|
      g.push_const :Rubinius
      g.find_const :Globals
      g.push_literal :"$_"
      g.send :[], 1
    end
  end
end
