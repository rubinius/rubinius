require File.dirname(__FILE__) + '/../spec_helper'

# Methods
# _id2ref, add_finalizer, call_finalizer, define_finalizer, each_object,
# finalizers, garbage_collect, remove_finalizer, undefine_finalizer

context "ObjectSpace class methods" do
  specify "_id2ref should convert an object id to a reference to the object" do
    example do
      s = "I am a string"
      r = ObjectSpace._id2ref(s.object_id)
      r == s
    end.should == true
  end
  specify "each_object should call the block once for each living, nonimmediate object in the Ruby process" do
    example do
      a = 102
      count = ObjectSpace.each_object(Numeric) {|x| p x}
      count
    end.should == 29
  end
end
