require File.expand_path('../../spec_helper', __FILE__)

describe "An And node" do
  relates "(a and b)" do
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
