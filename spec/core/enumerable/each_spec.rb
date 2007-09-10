require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Enumerable.each" do    
  it "be provided" do
    Numerous.new.respond_to?(:each).should == true
  end
  
  it "provide each element to the block" do 
    @b=[]
    Numerous.new.each { |i| @b << i }
    @b.should == [2, 5, 3, 6, 1, 4]
  end 
end
