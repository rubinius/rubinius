require File.dirname(__FILE__) + '/../../spec_helper'

describe "Tuple.new" do
  it "creates a Tuple of specified size" do
    Tuple.new(2).fields.should == 2
    Tuple.new(2).size.should == 2
    Tuple.new(2).length.should == 2
  end

  it "creates a Tuple of zero size" do
    Tuple.new(0).fields.should == 0
    Tuple.new(0).size.should == 0
    Tuple.new(0).length.should == 0
  end
end
