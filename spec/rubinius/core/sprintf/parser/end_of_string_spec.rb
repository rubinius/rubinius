require File.dirname(__FILE__) + '/../../../../spec_helper'

extension :rubinius do
  describe "Sprintf::Parser#end_of_string" do
    before(:each) do
      @helper = Sprintf::Parser.new("foo wow!", [1,2,3])
    end
  
    it "returns boolean indicating cursor position is off the end of the string" do
      while @helper.[] != ?!
        @helper.next
        @helper.end_of_string?.should == false
      end
      @helper.end_of_string?.should == false
      @helper.next
      @helper.end_of_string?.should == true
    end
  end
end
