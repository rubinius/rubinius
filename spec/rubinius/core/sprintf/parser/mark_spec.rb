require File.dirname(__FILE__) + '/../../../../spec_helper'

extension :rubinius do
  describe "Sprintf::Parser#mark" do
    before(:each) do
      @helper = Sprintf::Parser.new("foo wow!", [1,2,3])
    end
  
    it "mark & drop_mark_point records the current cursor point" do
      @helper.cursor.should == 0
      @helper.mark.should == 0
      @helper.next
      @helper.next
      @helper.cursor.should == 2
      @helper.mark.should == 0
      @helper.drop_mark_point
      @helper.mark.should == 2
    end
  end
end
