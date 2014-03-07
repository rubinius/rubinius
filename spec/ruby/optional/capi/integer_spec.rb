require File.expand_path('../spec_helper', __FILE__)

load_extension("bignum")

describe "CApiIntegerSpecs" do
  before :each do
    @s = CApiIntegerSpecs.new
  end

  describe "rb_integer_pack" do
  end
end
