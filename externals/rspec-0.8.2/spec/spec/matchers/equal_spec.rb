require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "should equal(expected)" do
  
  specify "should pass if target.equal?(expected)" do
    1.should equal(1)
  end
  
  specify "should fail unless target.equal?(expected)" do
    lambda {
      "1".should equal("1")
    }.should fail
  end
  
  specify "should provide message, expected and actual on failure" do
    matcher = equal("1")
    matcher.matches?("1")
    matcher.failure_message.should == ["expected \"1\", got \"1\" (using .equal?)", "1", "1"]
  end
  
end

context "should_not equal(expected)" do
  
  specify "should pass unless target.equal?(expected)" do
    "1".should_not equal("1")
  end
  
  specify "should fail if target.equal?(expected)" do
    lambda {
      1.should_not equal(1)
    }.should fail
  end
  
  specify "should provide message, expected and actual on failure" do
    matcher = equal(1)
    matcher.matches?(1)
    matcher.negative_failure_message.should == ["expected 1 not to equal 1 (using .equal?)", 1, 1]
  end
  
end
