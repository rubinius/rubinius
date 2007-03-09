require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "Spec::Expectations.fail_with with no diff" do
  setup do
    @old_differ = Spec::Expectations.differ
    Spec::Expectations.differ = nil
  end
  
  specify "should handle just a message" do
    lambda {
      Spec::Expectations.fail_with "the message"
    }.should_fail_with "the message"
  end
  
  specify "should handle an Array" do
    lambda {
      Spec::Expectations.fail_with ["the message","expected","actual"]
    }.should_fail_with "the message"
  end

  teardown do
    Spec::Expectations.differ = @old_differ
  end
end

context "Spec::Expectations.fail_with with diff" do
  setup do
    @old_differ = Spec::Expectations.differ
    @differ = mock("differ")
    Spec::Expectations.differ = @differ
  end
  
  specify "should not call differ if no expected/actual" do
    lambda {
      Spec::Expectations.fail_with "the message"
    }.should_fail_with "the message"
  end
  
  specify "should call differ if expected/actual are presented separately" do
    @differ.should_receive(:diff_as_string).and_return("diff")
    lambda {
      Spec::Expectations.fail_with "the message", "expected", "actual"
    }.should_fail_with "the message\nDiff:diff"
  end
  
  specify "should call differ if expected/actual are not strings" do
    @differ.should_receive(:diff_as_object).and_return("diff")
    lambda {
      Spec::Expectations.fail_with "the message", :expected, :actual
    }.should_fail_with "the message\nDiff:diff"
  end
  
  specify "should not call differ if expected or actual are procs" do
    @differ.should_not_receive(:diff_as_string)
    @differ.should_not_receive(:diff_as_object)
    lambda {
      Spec::Expectations.fail_with "the message", lambda {}, lambda {}
    }.should_fail_with "the message"
  end
  
  specify "should call differ if expected/actual are presented in an Array with message" do
    @differ.should_receive(:diff_as_string).with("actual","expected").and_return("diff")
    lambda {
      Spec::Expectations.fail_with(["the message", "expected", "actual"])
    }.should_fail_with /the message\nDiff:diff/
  end
  
  teardown do
    Spec::Expectations.differ = @old_differ
  end
end
