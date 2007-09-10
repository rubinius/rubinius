require File.dirname(__FILE__) + '/../../spec_helper'

float_modulo = shared "Float#%" do |cmd|
  describe "Float##{cmd}" do
    it "returns self modulo other" do
      6543.21.send(cmd, 137).should_be_close(104.21, TOLERANCE)
      5667.19.send(cmd, 0xffffffff).should_be_close(5667.19, TOLERANCE)
      6543.21.send(cmd, 137.24).should_be_close(92.9299999999996, TOLERANCE)

      6543.21.send(cmd, 137).should_be_close(6543.21.%(137), TOLERANCE)
      5667.19.send(cmd, 0xffffffff).should_be_close(5667.19.%(0xffffffff), TOLERANCE)
      6543.21.send(cmd, 137.24).should_be_close(6543.21.%(137.24), TOLERANCE)
    end

    it "does NOT raise ZeroDivisionError if other is zero" do
      1.0.send(cmd, 0).to_s.should == 'NaN'
      1.0.send(cmd, 0.0).to_s.should == 'NaN'
    end
  end
end

describe "Float#%" do
  it_behaves_like(float_modulo, :%)
end

describe "Float#modulo" do
  it_behaves_like(float_modulo, :modulo)
end
