require File.dirname(__FILE__) + '/../../spec_helper'

describe "A Call node using ConstantExpressions plugin" do
  relates "1 + 2" do
    parse do
      [:call, [:lit, 1], :+, [:arglist, [:lit, 2]]]
    end

    compile do |g|
      g.push 1
      g.push 2
      g.send :+, 1, false
    end

    compile :const_expr do |g|
      g.push_int 3
    end
  end

  relates "1 - 2" do
    parse do
      [:call, [:lit, 1], :-, [:arglist, [:lit, 2]]]
    end

    compile do |g|
      g.push 1
      g.push 2
      g.send :-, 1, false
    end

    compile :const_expr do |g|
      g.push_int -1
    end
  end

  relates "1 * 2" do
    parse do
      [:call, [:lit, 1], :*, [:arglist, [:lit, 2]]]
    end

    compile do |g|
      g.push 1
      g.push 2
      g.send :*, 1, false
    end

    compile :const_expr do |g|
      g.push_int 2
    end
  end

  relates "1 / 2" do
    parse do
      [:call, [:lit, 1], :/, [:arglist, [:lit, 2]]]
    end

    compile do |g|
      g.push 1
      g.push 2
      g.send :/, 1, false
    end

    compile :const_expr do |g|
      g.push_int 0
    end
  end

  relates "1 ** 2" do
    parse do
      [:call, [:lit, 1], :**, [:arglist, [:lit, 2]]]
    end

    compile do |g|
      g.push 1
      g.push 2
      g.send :**, 1, false
    end

    compile :const_expr do |g|
      g.push_int 1
    end
  end
end
