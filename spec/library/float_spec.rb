require File.dirname(__FILE__) + '/../spec_helper'

# %, *, **, +, -, -@, /, <, <=, <=>, ==, >, >=, abs, ceil, coerce,
# divmod, eql?, finite?, floor, hash, infinite?, modulo, nan?, round,
# to_f, to_i, to_int, to_s, truncate, zero?

context "Float" do
  specify "% should return self modulo other fixnum" do
    example do
      p [6543.21 % 137]
    end.should == '[104.21]'
  end
  
  specify "% should return self modulo other bignum" do
    example do
      p [5667.19 % 0xffffffff]
    end.should == '[5667.19]'
  end
  
  specify "% should return self modulo other float" do
    example do
      p [6543.21 % 137.24]
    end.should == '[92.9299999999996]'
  end
end
