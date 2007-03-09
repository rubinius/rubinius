require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "should be_close(expected, delta)" do
  specify "should pass if target == expected" do
    5.0.should be_close(5.0, 0.5)
  end
  specify "should pass if target < expected + delta" do
    5.49.should be_close(5.0, 0.5)
  end
  specify "should pass if target > expected - delta" do
    4.51.should be_close(5.0, 0.5)
  end
  specify "should fail if target == expected - delta" do
    lambda { 
      4.5.should be_close(5.0, 0.5)
    }.should fail_with("expected 5.0 +/- (<0.5), got 4.5")
  end
  specify "should fail if target < expected - delta" do
    lambda { 
      4.49.should be_close(5.0, 0.5)
    }.should fail_with("expected 5.0 +/- (<0.5), got 4.49")
  end
  specify "should fail if target == expected + delta" do
    lambda { 
      5.5.should be_close(5.0, 0.5)
    }.should fail_with("expected 5.0 +/- (<0.5), got 5.5")
  end
  specify "should fail if target > expected + delta" do
    lambda { 
      5.51.should be_close(5.0, 0.5)
    }.should fail_with("expected 5.0 +/- (<0.5), got 5.51")
  end
end