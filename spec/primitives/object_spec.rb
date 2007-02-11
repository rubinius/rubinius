require File.dirname(__FILE__) + '/../primitives_spec_helper'

context "The object_taint primitive" do
  specify "should return an identical string with a flag change" do
    'this'.prim.object_taint.should == 'this'
    # FIXME: can't verify flag change (wait for rspec _on_ rubinius?)
  end
end

