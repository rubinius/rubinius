require File.dirname(__FILE__) + '/../../../spec_helper'

context "should_be <" do
  specify "should pass when < operator returns true" do
    3.should_be < 4
  end

  specify "should fail when < operator returns false" do
    lambda { 3.should_be < 3 }.should_fail_with "expected < 3, got 3"
  end
end

context "should_be <=" do
  specify "should pass when <= operator returns true" do
    3.should_be <= 4
    4.should_be <= 4
  end

  specify "should fail when <= operator returns false" do
    lambda { 3.should_be <= 2 }.should_fail
  end
end

context "should_be >=" do
  specify "should pass when >= operator returns true" do
    4.should_be >= 4
    5.should_be >= 4
  end

  specify "should fail when >= operator returns false" do
    lambda { 3.should_be >= 4 }.should_fail
  end
end

context "should_be >" do
  specify "should pass when > operator returns true" do
    5.should_be > 4
  end

  specify "should fail when > operator returns false" do
    lambda { 3.should_be > 4 }.should_fail
  end
end

