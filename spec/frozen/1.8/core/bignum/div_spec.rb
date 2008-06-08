require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/divide'

describe "Bignum#div" do
  it_behaves_like(:bignum_divide, :div)

  ruby_bug "#", "1.8.6.114" do
    it "returns a result of integer division of self by a float argument" do
      @bignum.div(0xffff_ffff.to_f).should eql(2147483648)
      @bignum.div(@bignum.to_f).should eql(1)
      @bignum.div(-@bignum.to_f).should eql(-1)
    end
    it "raises FloatDomainError if the argument is a float zero" do
      lambda { @bignum.div(0.0) }.should raise_error(FloatDomainError, "Infinity")
      lambda { @bignum.div(-0.0) }.should raise_error(FloatDomainError, "-Infinity")
    end
  end
end
