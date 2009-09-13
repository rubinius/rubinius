require File.dirname(__FILE__) + '/../spec_helper'

describe "A Not node" do
  relates "(not true)" do
    parse do
      [:not, [:true]]
    end

    compile do |g|
      f = g.new_label
      t = g.new_label

      g.push :true
      g.git f

      g.push :true
      g.goto t

      f.set!
      g.push :false

      t.set!
    end
  end

  relates <<-ruby do
      a = 1
      b = !a
    ruby

    parse do
      [:block, [:lasgn, :a, [:lit, 1]], [:lasgn, :b, [:not, [:lvar, :a]]]]
    end

    compile do |g|
      invert = g.new_label
      done = g.new_label

      g.push 1
      g.set_local 0
      g.pop

      g.push_local 0
      g.git invert
      g.push :true
      g.goto done

      invert.set!
      g.push :false

      done.set!
      g.set_local 1
    end
  end
end
