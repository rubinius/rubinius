require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_be" do
  specify "should pass when objects are the same instance" do
    :apple.should_be(:apple)
  end

  specify "should fail when objects are not the same instance" do
    lambda { "apple".should_be("apple") }.should_fail
  end
end
