require File.expand_path('../../spec_helper', __FILE__)

describe "A Dot2 node" do
  relates "(a..b)" do
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
