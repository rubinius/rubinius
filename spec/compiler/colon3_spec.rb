require File.expand_path('../../spec_helper', __FILE__)

describe "A Colon3 node" do
  relates "::X" do
    compile do |g|
      g.push_cpath_top
      g.find_const :X
    end
  end
end
