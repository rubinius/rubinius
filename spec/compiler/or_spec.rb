require File.expand_path('../../spec_helper', __FILE__)

describe "An Or node" do
  relates "(a or b)" do
    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.dup

      lhs_true = g.new_label
      g.git lhs_true

      g.pop
      g.push :self
      g.send :b, 0, true

      lhs_true.set!
    end
  end

  or_complex = lambda do |g|
    j1 = g.new_label
    j2 = g.new_label
    j3 = g.new_label

    g.push :self
    g.send :a, 0, true
    g.dup
    g.git j1
    g.pop

    g.push :self
    g.send :b, 0, true
    j1.set!
    g.dup
    g.git j3
    g.pop

    g.push :self
    g.send :c, 0, true
    g.dup
    g.gif j2
    g.pop

    g.push :self
    g.send :d, 0, true

    j2.set!
    j3.set!
  end

  relates "((a || b) || (c && d))" do
    compile(&or_complex)
  end

  relates "((a or b) or (c and d))" do
    compile(&or_complex)
  end

  relates "() or a" do
    compile do |g|
      t = g.new_label
      g.push :nil
      g.dup
      g.git t
      g.pop
      g.push :self
      g.send :a, 0, true
      t.set!
    end
  end

  relates "a or ()" do
    compile do |g|
      t = g.new_label
      g.push :self
      g.send :a, 0, true
      g.dup
      g.git t
      g.pop
      g.push :nil
      t.set!
    end
  end
end
