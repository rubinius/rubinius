require File.dirname(__FILE__) + '/../../spec_helper'

describe "Exception#set_backtrace" do
  it "allows the user to set the backtrace to any array" do
    err = RuntimeError.new
    err.set_backtrace ["unhappy"]
    err.backtrace.should == ["unhappy"]
  end
end
