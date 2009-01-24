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
end
