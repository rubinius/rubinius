require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Sprintf::Parser#initialize" do
  before(:each) do
    @helper = Sprintf::Parser.new("foo wow!", [1,2,3])
  end

  it "initilize object" do
    @helper[].should == ?f
    @helper.cursor.should == 0
    @helper.mark.should == 0
    @helper.get_argument(0).should == 1
    @helper.get_argument(1).should == 2
    @helper.get_argument(2).should == 3
  end
end
