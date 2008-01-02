require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Sprintf::Parser#get_argument" do
  before(:each) do
    @helper = Sprintf::Parser.new("foo wow!", [1,2,3])
  end

  it "returns an items from the argument list" do
    @helper.get_argument(0).should == 1
    @helper.get_argument(1).should == 2
    @helper.get_argument(2).should == 3
    lambda { @helper.get_argument(4) }.should raise_error(ArgumentError)
  end
end
