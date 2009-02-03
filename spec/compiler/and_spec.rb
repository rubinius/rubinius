require File.dirname(__FILE__) + '/../spec_helper'

describe "An And node" do
  relates "(a and b)" do
    parse do
      [:and, [:call, nil, :a, [:arglist]], [:call, nil, :b, [:arglist]]]
    end

    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.dup

      lhs_true = g.new_label
      g.gif lhs_true

      g.pop
      g.push :self
      g.send :b, 0, true

      lhs_true.set!
    end
  end

  relates "() and a" do
    parse do
      [:and, [:nil], [:call, nil, :a, [:arglist]]]
    end

    compile do |g|
      f = g.new_label
      g.push :nil
      g.dup
      g.gif f
      g.pop
      g.push :self
      g.send :a, 0, true
      f.set!
    end
  end

  relates "a and ()" do
    parse do
      [:and, [:call, nil, :a, [:arglist]], [:nil]]
    end

    compile do |g|
      f = g.new_label
      g.push :self
      g.send :a, 0, true
      g.dup
      g.gif f
      g.pop
      g.push :nil
      f.set!
    end
  end
end
