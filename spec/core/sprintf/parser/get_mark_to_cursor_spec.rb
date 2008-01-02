require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Sprintf::Parser#get_mark_to_cursor" do
  before(:each) do
    @helper = Sprintf::Parser.new("foo wow!", [1,2,3])
  end

  it "returns the marked string segment" do
    @helper.cursor.should == 0
    @helper.next
    @helper.drop_mark_point
    @helper.next
    @helper.get_mark_to_cursor.should == 'o'
  end
end
