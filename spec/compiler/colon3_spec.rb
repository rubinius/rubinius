require File.dirname(__FILE__) + '/../spec_helper'

describe "A Colon3 node" do
  relates "::X" do
    compile do |g|
      g.push_cpath_top
      g.find_const :X
    end
  end
end
