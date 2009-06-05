require File.dirname(__FILE__) + '/../../spec_helper'

describe "ObjectSpace.each_object" do
  it "calls the block once for each living, nonimmediate object in the Ruby process" do
    class ObjectSpaceSpecEachObject; end
    new_obj = ObjectSpaceSpecEachObject.new

    count = ObjectSpace.each_object(ObjectSpaceSpecEachObject) {}
    count.should == 1
    # this is needed to prevent the new_obj from being GC'd too early
    new_obj.should_not == nil
  end
  
  ruby_version_is '1.8.7' do
    it "returns an enumerator if not given a block" do
      class ObjectSpaceSpecEachOtherObject; end
      new_obj = ObjectSpaceSpecEachOtherObject.new

      counter = ObjectSpace.each_object(ObjectSpaceSpecEachOtherObject)
      counter.should be_kind_of(enumerator_class)
      counter.each{}.should == 1
      # this is needed to prevent the new_obj from being GC'd too early
      new_obj.should_not == nil
    end
  end
end
