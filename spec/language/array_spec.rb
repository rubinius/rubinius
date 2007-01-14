require File.dirname(__FILE__) + '/../spec_helper'

context "Array literal" do
  specify "[] should return a new array populated with the given elements" do
    example do
      [1, 2, 3, 'a', 'b', 5]
    end.should == [1, 2, 3, "a", "b", 5]
  end
end
