require File.dirname(__FILE__) + '/../../spec_helper.rb'

require 'spec/expectations/differs/default'

describe "should ==" do
  
  it "should delegate message to target" do
    subject = "apple"
    subject.should_receive(:==).with("apple").and_return(true)
    subject.should == "apple"
  end
  
  it "should fail when target.==(actual) returns false" do
    subject = "apple"
    Spec::Expectations.should_receive(:fail_with).with(%[expected "orange", got "apple" (using ==)], "orange", "apple")
    subject.should == "orange"
  end

end

describe "should_not ==" do
  
  it "should delegate message to target" do
    subject = "orange"
    subject.should_receive(:==).with("apple").and_return(false)
    subject.should_not == "apple"
  end
  
  it "should fail when target.==(actual) returns false" do
    subject = "apple"
    Spec::Expectations.should_receive(:fail_with).with(%[expected not == "apple", got "apple"], "apple", "apple")
    subject.should_not == "apple"
  end

end

  