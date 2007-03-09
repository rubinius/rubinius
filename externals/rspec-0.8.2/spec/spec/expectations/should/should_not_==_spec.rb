require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_not ==" do
  specify "should pass objects are not ==" do
    lambda do
      "apple".should_not == "cadillac"
    end.should_pass
  end
  
  specify "should fail when objects are ==" do
    lambda do
      "apple".should_not == "apple"
    end.should_fail_with "expected not == \"apple\", got \"apple\""
  end
end
