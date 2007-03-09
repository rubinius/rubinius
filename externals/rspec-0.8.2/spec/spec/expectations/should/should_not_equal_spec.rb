require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_not_equal" do
  specify "should pass when objects are not the same instance" do
    "apple".should_not_equal("apple")
  end

  specify "should fail when objects are the same instance" do
    lambda { :apple.should_not_equal(:apple) }.should_fail_with "expected :apple not to equal :apple (using .equal?)"
  end
end
