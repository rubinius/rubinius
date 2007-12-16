require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#each" do
  it "yields each element to the block" do
    a = []
    x = [1, 2, 3]
    x.each { |item| a << item }.equal?(x).should == true
    a.should == [1, 2, 3]
  end
  
  it "yields each element to the block even if the array is changed during iteration" do
    a = [1, 2, 3, 4, 5]
    b = []
    a.each {|x| b << x; a << x+5 if (x%2).zero? }
    b.should == [1, 2, 3, 4, 5, 7, 9]
  end

  it "should support array explosion" do
    a = [[1, 2], [3, 4]]
    b = []
    
    a.each { |x, y| b << x }
    b.should == [1, 3]

    b = []
    a.each { |x, y| b << y }
    b.should == [2, 4]
    
    b = []
    a.each { |x, y| b << x + y }
    b.should == [3, 7]
  end
end
