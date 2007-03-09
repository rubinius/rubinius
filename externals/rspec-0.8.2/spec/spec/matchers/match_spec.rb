require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "should match(expected)" do
  specify "should pass when target (String) matches expected (Regexp)" do
    "string".should match(/tri/)
  end

  specify "should fail when target (String) matches expected (Regexp)" do
    lambda {
      "string".should match(/rings/)
    }.should fail
  end

  specify "should provide message, expected and actual on failure" do
    matcher = match(/rings/)
    matcher.matches?("string")
    matcher.failure_message.should == ["expected \"string\" to match /rings/", /rings/, "string"]
  end
end

context "should_not match(expected)" do
  specify "should pass when target (String) matches expected (Regexp)" do
    "string".should_not match(/rings/)
  end

  specify "should fail when target (String) matches expected (Regexp)" do
    lambda {
      "string".should_not match(/tri/)
    }.should fail
  end

  specify "should provide message, expected and actual on failure" do
    matcher = match(/tri/)
    matcher.matches?("string")
    matcher.negative_failure_message.should == ["expected \"string\" not to match /tri/", /tri/, "string"]
  end
end
