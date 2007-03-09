require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_be false" do
  specify "should fail when target is true" do
    lambda do
      true.should_be false
    end.should_fail_with "expected false, got true (using .equal?)"
  end

  specify "should fail when target is a number" do
    lambda do
      5.should_be false
    end.should_fail
  end

  specify "should fail when target is a some random object" do
    lambda do
      self.should_be false
    end.should_fail
  end

  specify "should fail when target is a string" do
    lambda do
      "false".should_be false
    end.should_fail
  end

  specify "should fail when target is nil" do
    lambda do
      nil.should_be false
    end.should_raise
  end

  specify "should pass when target is false" do
    lambda do
      false.should_be false
    end.should_not_raise
  end
end
