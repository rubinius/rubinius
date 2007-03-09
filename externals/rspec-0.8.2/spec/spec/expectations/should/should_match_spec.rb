require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_match" do
  specify "should pass when objects match" do
    "hi aslak".should_match(/aslak/)
  end

  specify "should fail objects do not match" do
    lambda do
      "hi aslak".should_match(/steve/)
    end.should_fail
  end
end

context "should_not_match" do
  specify "should fail when objects match" do
    lambda do
      "hi aslak".should_not_match(/aslak/)
    end.should_fail
  end

  specify "should pass objects do not match" do
    "hi aslak".should_not_match(/steve/)
  end
end
