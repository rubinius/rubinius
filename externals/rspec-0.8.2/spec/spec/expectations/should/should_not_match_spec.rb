require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_not_match" do
  specify "should not raise when objects do not match" do
    "hi aslak".should_not_match(/steve/)
  end

  specify "should raise when objects match" do
    lambda { "hi aslak".should_not_match(/aslak/) }.should_fail
  end
end
