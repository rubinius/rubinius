require File.dirname(__FILE__) + '/../../spec_helper'

describe "Integer#upto [stop] when self and stop are Fixnums" do
  it "does not yield when stop is less than self" do
    result = []
    5.upto(4) { |x| result << x }
    result.should == []
  end
  
  it "yields once when stop equals self" do
    result = []
    5.upto(5) { |x| result << x }
    result.should == [5]
  end
  
  it "yields while decreasing self until it is less than stop" do
    result = []
    2.upto(5) { |x| result << x }
    result.should == [2, 3, 4, 5]
  end
end
