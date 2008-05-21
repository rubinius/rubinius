require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures'

describe "ObjectSpace#define_finalizer" do
  it "takes a proc and ignores the block" do
    lambda {
      ObjectSpace.define_finalizer("", proc { 1 }) do
        2
      end
    }.should_not raise_error(ArgumentError)
  end

  it "complains if the action doesn't respond to call" do
    lambda { 
      ObjectSpace.define_finalizer("", 3)
    }.should raise_error(ArgumentError)
  end

  it "causes calls with the object_id when the object is dead" do
    handler = ObjectSpaceFixtures.make_finalizer
    ObjectSpace.define_finalizer ObjectSpaceFixtures.garbage, handler
    3.times { GC.start }

    ObjectSpaceFixtures.last_objid == ObjectSpaceFixtures.garbage_objid
  end
end
