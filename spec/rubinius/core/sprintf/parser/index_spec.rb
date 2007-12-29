require File.dirname(__FILE__) + '/../../../../spec_helper'

describe "Sprintf::Parser#index" do
  before(:each) do
    @helper = Sprintf::Parser.new("foo wow!", [1,2,3])
  end

  it "index returns the character position (or nil) of the searched character" do
    @helper.index(?w).should == 4
    @helper.cursor.should == 4
    @helper.rewind
    @helper.index(?&).should == nil
    @helper.cursor.should == 8
    @helper.end_of_string?.should == true
  end
end
