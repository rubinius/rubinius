require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/helpers/bignum'

describe Object, "#bignum_value" do
  it "returns a value that is an instance of Bignum on any platform" do
    bignum_value.should ==  0x8000_0000_0000_0000
  end

  it "returns the default value incremented by the argument" do
  end
end
