require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

@bignum_divide = shared "Bignum#/" do |cmd|
  describe "Bignum##{cmd}" do
    it "returns self divided by other" do
      a = BignumHelper.sbm(88)
      a.send(cmd, 4).should_be_close(268435478, TOLERANCE)
      a.send(cmd, 16.2).should_be_close(66280364.9382716, TOLERANCE)
      a.send(cmd, BignumHelper.sbm(2)).to_s.should == '1'
    end

    it "raises ZeroDivisionError if other is zero and not a Float" do
      should_raise(ZeroDivisionError) { BignumHelper.sbm.send cmd, 0 }
    end

    it "does NOT raise ZeroDivisionError if other is zero and is a Float" do
      a = BignumHelper.sbm(5_221)
      b = BignumHelper.sbm(25)
      a.send(cmd, 0.0).to_s.should == 'Infinity'
      b.send(cmd, -0.0).to_s.should == '-Infinity'
    end
  end
end

describe "Bignum#/" do
  it_behaves_like(@bignum_divide, :/)
end
