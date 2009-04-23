require File.dirname(__FILE__) + '/../spec_helper'

describe "A Defined node" do
  relates "defined? $x" do
    parse do
      [:defined, [:gvar, :$x]]
    end

    compile do |g|
      t = g.new_label
      f = g.new_label

      g.push_const :Rubinius
      g.find_const :Globals
      g.push_literal :$x
      g.send :key?, 1
      g.git t

      g.push :nil
      g.goto f

      t.set!
      g.push_literal "global-variable"

      f.set!
    end
  end
end
