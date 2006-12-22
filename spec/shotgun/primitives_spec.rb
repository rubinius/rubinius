require File.dirname(__FILE__) + '/../spec_helper'

context "Fixnum" do
  
  specify "% should return self modulo other" do
    example do
      p [451 % 2, 93 % 3]
    end.should == '[1, 0]'
  end
end
