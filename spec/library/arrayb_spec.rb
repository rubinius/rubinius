require File.dirname(__FILE__) + '/../spec_bhelper'

context "Array" do
  specify "& should create an array with elements common to both arrays" do
    example do
      [ 1, 1, 3, 5 ] & [ 1, 2, 3 ]
    end.should == [1, 3]
  end
  
end