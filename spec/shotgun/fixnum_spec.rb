require File.dirname(__FILE__) + '/../spec_helper'

context "Fixnum instance method" do
  specify "size should return the number of bytes in the machine representation of self" do
    example do
      [-1.size, 0.size, 4091.size]
    end.should == [4, 4, 4]
  end
end
