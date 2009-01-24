require File.dirname(__FILE__) + '/../spec_helper'

describe "A Iasgn node" do
  relates "@a = 4" do
    parse do
      [:iasgn, :@a, [:lit, 4]]
    end

    compile do |g|
      g.push 4
      g.set_ivar :@a
    end
  end

  relates "@a = *[1]" do
    parse do
      [:iasgn, :@a, [:svalue, [:splat, [:array, [:lit, 1]]]]]
    end

    # iasgn splat
  end
end
