require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_equal" do
  specify "should pass when objects are the same instance" do
    :apple.should_equal(:apple)
  end

  specify "should fail when objects are not the same instance" do
    lambda { "apple".should_equal("apple") }.should_fail_with "expected \"apple\", got \"apple\" (using .equal?)"
  end
end