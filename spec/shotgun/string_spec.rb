require File.dirname(__FILE__) + '/../spec_helper'

context "String instance method" do
  specify "prefix? should be true if string begins with argument" do
    example do
      p [ "blah".prefix?("bl"),
          "blah".prefix?("fo"),
          "go".prefix?("gogo") ]
    end.should == '[true, false, false]'
  end  

  specify "substring should return the portion of string specified by index, length" do
    example do
      p [ "blah".substring(0, 2),
          "blah".substring(0, 4),
          "blah".substring(2, 2) ]
    end.should == '["bl", "blah", "ah"]'
  end  
end

context "String implementation" do
  specify "underlying storage should have the correct size (space for last \0 and multiple of 4)" do
    example do
      p "hell".data.size
    end.should == "8"
  end
end
