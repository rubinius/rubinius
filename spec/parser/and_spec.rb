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
end
