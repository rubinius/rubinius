require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "should eql(expected)" do
  
  specify "should pass if target.eql?(expected)" do
    1.should eql(1)
  end
  
  specify "should fail unless target.eql?(expected)" do
    lambda {
      1.should eql("1")
    }.should fail
  end
  
  specify "should provide message, expected and actual on failure" do
    matcher = eql("1")
    matcher.matches?(1)
    matcher.failure_message.should == ["expected \"1\", got 1 (using .eql?)", "1", 1]
  end
  
end

context "should_not eql(expected)" do
  
  specify "should pass unless target.eql?(expected)" do
    1.should_not eql("1")
  end
  
  specify "should fail if target.eql?(expected)" do
    lambda {
      1.should_not eql(1)
    }.should fail
  end
  
  specify "should provide message, expected and actual on failure" do
    matcher = eql(1)
    matcher.matches?(1)
    matcher.negative_failure_message.should == ["expected 1 not to equal 1 (using .eql?)", 1, 1]
  end
  
end
