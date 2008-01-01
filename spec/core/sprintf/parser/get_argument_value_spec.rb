require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Sprintf::Parser#get_argument_value" do
  before(:each) do
    @helper = Sprintf::Parser.new("foo wow!", [1,2,3])
  end

  it "returns an argument value from a *nn$ or * string" do
    @helper = Sprintf::Parser.new("*2$ %* %*789", [1, 2, 3])
    @helper.get_argument_value.should == 2
    @helper.index(?%)
    @helper.next
    @helper.get_argument_value.should == 2
    @helper.cursor.should == 6
    @helper.index(?%)
    @helper.next
    begin
      @helper.get_argument_value
    rescue ArgumentError
      $!.message
    end.should == "malformed format string - %.[0-9]"
  end
end
