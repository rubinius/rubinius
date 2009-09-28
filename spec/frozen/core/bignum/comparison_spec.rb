require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum#<=>" do
  before(:each) do
    @bignum = bignum_value(96)
    @inf = 1/0.0
  end

  it "returns -1 when self is less than the given argument" do
    (-@bignum <=> @bignum).should == -1
    (-@bignum <=> -1).should == -1
    (-@bignum <=> -4.5).should == -1
  end

  it "returns 0 when self is equal to the given argument" do
    (@bignum <=> @bignum).should == 0
    (-@bignum <=> -@bignum).should == 0
  end

  it "returns 1 when self is greater than the given argument" do
    (@bignum <=> -@bignum).should == 1
    (@bignum <=> 1).should == 1
    (@bignum <=> 4.5).should == 1
  end

  it "returns nil when the given argument is not an Integer" do
    (@bignum <=> mock('str')).should == nil
    (@bignum <=> 'test').should == nil
  end

  # TODO: Remove duplicate bug guards when ruby_bug is updated.
  ruby_bug "[ruby-dev:38672] [Bug #1645]", "1.8.7.174" do
    # The 4 tests below are taken from matz's revision 23730 for Ruby trunk
    #
    it "returns 1 when self is Infinity and other is a Bignum" do
      (@inf <=> Float::MAX.to_i*2).should == 1
    end

    it "returns 1 when self is negative and other is -Infinity" do
      (-Float::MAX.to_i*2 <=> -@inf).should == 1
    end

    it "returns -1 when self is -Infinity and other is negative" do
      (-@inf <=> -Float::MAX.to_i*2).should == -1
    end

    it "returns -1 when self is negative and other is Infinty" do
      (-Float::MAX.to_i*2 <=> @inf).should == -1
    end
  end

  ruby_bug "[ruby-dev:38672] [Bug #1645]", "1.8.7" do
    # The 4 tests below are taken from matz's revision 23730 for Ruby trunk
    #
    it "returns 1 when self is Infinity and other is a Bignum" do
      (@inf <=> Float::MAX.to_i*2).should == 1
    end

    it "returns 1 when self is negative and other is -Infinity" do
      (-Float::MAX.to_i*2 <=> -@inf).should == 1
    end

    it "returns -1 when self is -Infinity and other is negative" do
      (-@inf <=> -Float::MAX.to_i*2).should == -1
    end

    it "returns -1 when self is negative and other is Infinty" do
      (-Float::MAX.to_i*2 <=> @inf).should == -1
    end
  end
end
