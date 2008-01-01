require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Sprintf::Parser#get_next_argument" do
  before(:each) do
    @helper = Sprintf::Parser.new("foo wow!", [1,2,3])
  end

  it "returns the next item from the argument list" do
    @helper.get_next_argument.should == 1
    @helper.get_next_argument.should == 2
    @helper.get_next_argument.should == 3

    lambda {
      @helper.get_next_argument
      @helper.get_next_argument
      @helper.get_next_argument
      @helper.get_next_argument
    }.should raise_error(ArgumentError)
  end
end
