require File.dirname(__FILE__) + '/../spec_helper'

describe "A Colon3 node" do
  relates "::X" do
    parse do
      [:colon3, :X]
    end

    compile do |g|
      g.push_cpath_top
      g.find_const :X
    end
  end
end
