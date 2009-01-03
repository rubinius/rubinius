require File.dirname(__FILE__) + '/../../spec_helper'
require 'complex'

describe "Complex#inspect" do
  it %{returns "Complex(real, image)"} do
    # Guard against the Mathn library
    conflicts_with :Prime do
      Complex(1).inspect.should == "Complex(1, 0)"
      Complex(7).inspect.should == "Complex(7, 0)"
    end

    Complex(-1, 4).inspect.should == "Complex(-1, 4)"
    Complex(-7, 6.7).inspect.should == "Complex(-7, 6.7)"
  end
end
