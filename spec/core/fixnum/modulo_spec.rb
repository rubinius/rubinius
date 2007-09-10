require File.dirname(__FILE__) + '/../../spec_helper'

fixnum_modulo = shared "Fixnum#%" do |cmd|
  describe "Fixnum##{cmd}" do
    it "returns self modulo other" do
      451.send(cmd, 2).should == 1
      32.send(cmd, 16).should == 0
    end

    it "coerces fixnum and return self modulo other" do
      93.send(cmd, 3.2).to_s.should == '0.199999999999995'
      120.send(cmd, -4.5).to_s.should == '-1.5'
    end

    it "raises ZeroDivisionError if other is zero and not a Float" do
      should_raise(ZeroDivisionError) { 1.send(cmd, 0) }
    end

    it "does NOT raise ZeroDivisionError if other is zero and is a Float" do
      1.send(cmd, 0.0).to_s.should == 'NaN'
      1.send(cmd, -0.0).to_s.should == 'NaN'
    end
  end
end

describe "Fixnum#%" do
  it_behaves_like(fixnum_modulo, :%)
end

describe "Fixnum#modulo" do
  it_behaves_like(fixnum_modulo, :modulo)
end
