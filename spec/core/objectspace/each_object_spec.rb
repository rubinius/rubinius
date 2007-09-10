require File.dirname(__FILE__) + '/../../spec_helper'

context "ObjectSpace.each_object" do
  specify "calls the block once for each living, nonimmediate object in the Ruby process" do
    class ObjectSpaceSpecEachObject; end
    ObjectSpaceSpecEachObject.new

    count = ObjectSpace.each_object(ObjectSpaceSpecEachObject) {}
    count.should == 1
  end
end
