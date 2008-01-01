require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Sprintf::Parser#[]" do
  before(:each) do
    @helper = Sprintf::Parser.new("foo wow!", [1,2,3])
  end

  it "returns the character at index" do
    @helper[].should == ?f
    @helper[1].should == ?o
    @helper.next
    @helper[].should == ?o
    @helper[3].should == ?w
  end
end
