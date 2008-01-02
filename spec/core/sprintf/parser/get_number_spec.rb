require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Sprintf::Parser#get_number" do
  before(:each) do
    @helper = Sprintf::Parser.new("foo wow!", [1,2,3])
  end

  it "returns a numerical value from a number string at the cursor position" do
    @helper = Sprintf::Parser.new("12345 %ABCD %789", 1)
    @helper.get_number.should == 12345
    @helper.cursor.should == 5
    @helper.index(?%)
    @helper.next
    @helper.get_number.should == 0
    @helper.cursor.should == 7
    @helper.index(?%)
    @helper.next
    @helper[].should == ?7
    begin
      @helper.get_number
    rescue ArgumentError 
      $!.message
    end.should == "malformed format string - %.[0-9]"
  end
end
