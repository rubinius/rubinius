require File.dirname(__FILE__) + '/../spec_helper'

context "String" do
  specify "index with substring arg should return substring when it exists" do
    example do
      p "hello".index('e')
    end.should == "1"
  end
end