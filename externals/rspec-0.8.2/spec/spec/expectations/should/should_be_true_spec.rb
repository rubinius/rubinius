require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_be true" do
  specify "should fail when object is false" do
    lambda { false.should_be(true) }.should_fail
  end

  specify "should fail when object is nil" do
    lambda { nil.should_be(true) }.should_fail
  end

  specify "should fail when object is a number" do
    lambda { 5.should_be(true) }.should_fail
  end

  specify "should fail when object is a some random object" do
    lambda { self.should_be(true) }.should_fail
  end

  specify "should fail when object is a string" do
    lambda { "true".should_be(true) }.should_raise
  end

  specify "should pass when object is true" do
    true.should_be(true)
  end
end
