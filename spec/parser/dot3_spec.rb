require File.dirname(__FILE__) + '/../spec_helper'

describe "A Dot3 node" do
  relates "(a...b)" do
    parse do
      [:dot3, [:call, nil, :a, [:arglist]], [:call, nil, :b, [:arglist]]]
    end

    # dot3
  end
end
