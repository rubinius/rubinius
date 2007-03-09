require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_be_close" do
  specify "should not raise when values are within bounds" do
    3.5.should_be_close(3.5, 0.5)
    3.5.should_be_close(3.1, 0.5)
    3.5.should_be_close(3.01, 0.5)
    3.5.should_be_close(3.9, 0.5)
    3.5.should_be_close(3.99, 0.5)
  end

  specify "should raise when values are outside bounds" do
    lambda { 3.5.should_be_close(3.0, 0.5) }.should_fail_with "expected 3.0 +/- (<0.5), got 3.5"
    lambda { 3.5.should_be_close(2.0, 0.5) }.should_fail
    lambda { 3.5.should_be_close(3.0, 0.5) }.should_fail
    lambda { 3.5.should_be_close(4.0, 0.5) }.should_fail
  end
end
