require File.dirname(__FILE__) + '/../spec_helper'

describe "An And node" do
  relates "(a and b)" do
    parse do
      [:and, [:call, nil, :a, [:arglist]], [:call, nil, :b, [:arglist]]]
    end

    # and
  end
end
