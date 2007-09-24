require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#each" do
  it "yields each element to the block" do
    a = []
    x = [1, 2, 3]
    x.each { |item| a << item }.equal?(x).should == true
    a.should == [1, 2, 3]
  end
  
  it "should support array explosion" do
    a = [[1,2], [3,4]]
    b = c = d = []
    a.each {|x,y| b << x }
    b.should == [1,3]
    a.each {|x,y| c << y }
    c.should == [2,4]    
    a.each {|x,y| d << x + y }
    d.should == [4, 6]
  end
end
