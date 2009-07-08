require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/helpers/infinity'

describe Object, "#infinity_value" do
  it "returns Infinity" do
    infinity_value.infinite?.should == 1
  end
end
