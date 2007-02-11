require File.dirname(__FILE__) + '/../primitives_spec_helper'

context "The object_taint primitive" do
  specify "should return an identical string with a flag change" do
    'this'.prim.object_taint.should == 'this'
    'this'.prim.object_taint.tainted?.should == true
  end
end

