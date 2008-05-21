require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/divide'

describe "Bignum#div" do
  it_behaves_like(:bignum_divide, :div)

  ruby_bug "#", "1.8.6.114" do
    it "returns self divided by float" do
      @bignum.div(0xffff_ffff.to_f).should == 2147483648
    end
  end
end
