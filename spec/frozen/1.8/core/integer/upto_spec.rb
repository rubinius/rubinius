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

  ruby_version_is "" ... "1.8.7" do
    it "raises a LocalJumpError when no block given" do
      lambda { 2.upto(5) }.should raise_error(LocalJumpError)
    end
  end

  ruby_version_is "1.8.7" do
    it "returns an Enumerator" do
      result = []
      
      enum = 2.upto(5)
      enum.each { |i| result << i }
      
      result.should == [2, 3, 4, 5]
    end
  end
end
