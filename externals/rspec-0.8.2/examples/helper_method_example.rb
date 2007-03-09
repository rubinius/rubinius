require File.dirname(__FILE__) + '/spec_helper'

context "a context with helper a method" do
  def helper_method
    "received call"
  end
  
  specify "should make that method available to specs" do
    helper_method.should == "received call"
  end
end