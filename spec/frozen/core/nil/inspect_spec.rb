require File.dirname(__FILE__) + '/../../spec_helper'

describe "NilClass#inspect" do
  it "returns the string 'nil'" do
    nil.inspect.should == "nil"
  end
end
