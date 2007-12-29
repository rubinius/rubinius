require File.dirname(__FILE__) + '/../../../../spec_helper'

describe "Sprintf::Parser#cursor" do
  before(:each) do
    @helper = Sprintf::Parser.new("foo wow!", [1,2,3])
  end

  it "returns the absolute string position" do
    @helper.cursor.should == 0
    @helper.next
    @helper.cursor.should == 1
  end
end
