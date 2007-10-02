require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

@kernel_object_id = shared "Kernel#object_id" do |cmd|
  describe "Object##{cmd}" do
    # #object_id and #__id__ are aliases, so we only need one function
    # that tests both methods
    it "return an Integer" do
      Object.new.send(cmd).class.should == Fixnum      
      nil.send(cmd).class.should == Fixnum      
    end
  
    it "return same number will be returned on all calls to id for a given object" do
      o1 = Object.new
      o2 = Object.new
      o1.send(cmd).should == o1.send(cmd)
    end
  
    it "return same number will be returned on all calls to id for a given object" do
      o1 = Object.new
      o2 = Object.new
      o1.send(cmd).should_not == o2.send(cmd)
    end
    it "" do
    end
    
  end
end

describe "Object.object_id" do
  it_behaves_like(@kernel_object_id, :object_id) 
end
