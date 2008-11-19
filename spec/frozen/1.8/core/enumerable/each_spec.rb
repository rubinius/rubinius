require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Enumerable.each" do    
  it "is provided" do
    EnumerableSpecs::Numerous.new.respond_to?(:each).should == true
  end
  
  it "provides each element to the block" do 
    @b=[]
    EnumerableSpecs::Numerous.new.each { |i| @b << i }
    @b.should == [2, 5, 3, 6, 1, 4]
  end 
end
