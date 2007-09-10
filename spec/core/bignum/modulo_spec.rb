require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

@bignum_modulo = shared "Bignum#modulo" do |cmd|
  describe "Bignum##{cmd}" do
    it "returns self modulo other" do
      a = BignumHelper.sbm(2_222)
      a.send(cmd, 5).should == 1
      a.send(cmd, 2.22).inspect.should == '0.419999905491539'
      a.send(cmd, BignumHelper.sbm).should == 2222
    end

    it "raises ZeroDivisionError if other is zero and not a Float" do
      should_raise(ZeroDivisionError) { BignumHelper.sbm.send(cmd, 0) }
    end

    it "% should NOT raise ZeroDivisionError if other is zero and is a Float" do
      a = BignumHelper.sbm(5_221)
      b = BignumHelper.sbm(25)
      a.send(cmd, 0.0).to_s.should == 'NaN'
      b.send(cmd, -0.0).to_s.should == 'NaN'
    end
  end
end

describe "Bignum#%" do
  it_behaves_like(@bignum_modulo, :%)
end

describe "Bignum#modulo" do
  it_behaves_like(@bignum_modulo, :modulo)
end
