require File.expand_path('../../spec_helper', __FILE__)

describe "A Valias node" do
  relates "alias $y $x" do
    compile do |g|
      g.push_rubinius
      g.find_const :Globals
      g.push_literal :$x
      g.push_literal :$y
      g.send :add_alias, 2
    end
  end
end
