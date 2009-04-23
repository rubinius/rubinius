require File.dirname(__FILE__) + '/../spec_helper'

describe "A Valias node" do
  relates "alias $y $x" do
    parse do
      [:valias, :$y, :$x]
    end

    compile do |g|
      g.push_const :Rubinius
      g.find_const :Globals
      g.push_literal :$x
      g.push_literal :$y
      g.send :add_alias, 2
    end
  end
end
