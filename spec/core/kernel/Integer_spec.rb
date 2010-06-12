require File.expand_path('../../../spec_helper', __FILE__)

describe "Kernel.Integer" do
  it "calls to_inum on strings with a base of 0 and with checks on" do
    the_answer = "0x42"
    the_answer.should_receive(:to_inum).with(0, true).and_return(66)
    Integer(the_answer).should == 66
  end
end
