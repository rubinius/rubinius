require File.dirname(__FILE__) + '/../spec_helper'

context "String" do
  specify "index with substring arg should return substring when it exists" do
    example do
      p "hello".index('e')
    end.should == "1"
  end

  specify "underlying storage should have the correct size (space for last \0 and multiple of 4)" do
    example do
      p "hell".data.size
    end.should == "8"
  end
end
