require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_not_eql" do
  specify "should pass when objects are not equal (by value)" do
    "apples".should_not_eql("cadillacs")
  end

  specify "should fail when objects are equal (by value)" do
    lambda { "apple".should_not_eql("apple") }.should_fail_with "expected \"apple\" not to equal \"apple\" (using .eql?)"
  end
end
