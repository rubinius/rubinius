require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_eql" do
  specify "should pass when objects are equal (by value)" do
    "apples".should_eql("apples")
  end

  specify "should fail when objects are not equal (by value)" do
    lambda { "apples".should_eql("cadillacs") }.should_fail_with "expected \"cadillacs\", got \"apples\" (using .eql?)"
  end
end
