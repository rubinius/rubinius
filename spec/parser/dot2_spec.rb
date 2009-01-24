require File.dirname(__FILE__) + '/../spec_helper'

describe "A Dot2 node" do
  relates "(a..b)" do
    parse do
      [:dot2, [:call, nil, :a, [:arglist]], [:call, nil, :b, [:arglist]]]
    end

    compile do |g|
      g.push_cpath_top
      g.find_const :Range
      g.push :self
      g.send :a, 0, true
      g.push :self
      g.send :b, 0, true
      g.send :new, 2
    end
  end
end
