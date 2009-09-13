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

  relates <<-ruby do
      a = 1
      @a = a
    ruby

    parse do
      [:block,
        [:lasgn, :a, [:lit, 1]],
        [:iasgn, :@a, [:lvar, :a]]]
    end

    compile do |g|
      g.push 1
      g.set_local 0
      g.pop
      g.push_local 0
      g.set_ivar :@a
    end
  end
end
