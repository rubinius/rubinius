require File.dirname(__FILE__) + '/../../spec_helper'

float_to_i = shared "Float#to_i" do |cmd|
  describe "Float##{cmd}" do
    it "returns self truncated to an Integer" do
      899.2.send(cmd).should == 899
      -1.122256e-45.send(cmd).should == 0
      5_213_451.9201.send(cmd).should == 5213451
    end
  end
end

describe "Float#to_i" do
  it_behaves_like(float_to_i, :to_i)
end
