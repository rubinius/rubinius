require File.dirname(__FILE__) + '/../../spec_helper.rb'

describe "should =~" do
  
  it "should delegate message to target" do
    subject = "foo"
    subject.should_receive(:=~).with(/oo/).and_return(true)
    subject.should =~ /oo/
  end
  
  it "should fail when target.=~(actual) returns false" do
    subject = "fu"
    subject.should_receive(:=~).with(/oo/).and_return(false)
    Spec::Expectations.should_receive(:fail_with).with(%[expected =~ /oo/, got "fu"], /oo/, "fu")
    subject.should =~ /oo/
  end

end

describe "should_not =~" do
  
  it "should delegate message to target" do
    subject = "fu"
    subject.should_receive(:=~).with(/oo/).and_return(false)
    subject.should_not =~ /oo/
  end
  
  it "should fail when target.=~(actual) returns false" do
    subject = "foo"
    subject.should_receive(:=~).with(/oo/).and_return(true)
    Spec::Expectations.should_receive(:fail_with).with(%[expected not =~ /oo/, got "foo"], /oo/, "foo")
    subject.should_not =~ /oo/
  end

end

