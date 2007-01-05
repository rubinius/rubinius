require File.dirname(__FILE__) + '/../spec_helper'

# %, *, **, +, -, -@, /, <, <=, <=>, ==, >, >=, abs, ceil, coerce,
# divmod, eql?, finite?, floor, hash, infinite?, modulo, nan?, round,
# to_f, to_i, to_int, to_s, truncate, zero?

context "Float" do
  specify "% should return self modulo other" do
    example do
      p [6543.21 % 137, 5667.19 % 0xffffffff, 6543.21 % 137.24]
    end.should == '[104.21, 5667.19, 92.9299999999996]'
  end
  
  specify "* should return self multiplied by other" do
    example do
      p [4923.98221 * 2, 256.4096 * 0xffffffff, 6712.5 * 0.25]
    end.should == '[9847.96442, 1101270846124.03, 1678.125]'
  end
  
  specify "** should return self raise to the other power" do
    example do
      p [2.3 ** 3, 5.2 ** -1, 9.5 ** 0.5, 9.5 ** 0xffffffff]
    end.should == '[12.167, 0.192307692307692, 3.08220700148449, Infinity]'
  end
  
  specify "+ should return self plus other" do
    example do
      p [491.213 + 2, 9.99 + 0xffffffff, 1001.99 + 5.219]
    end.should == '[493.213, 4294967304.99, 1007.209]'
  end

  specify "- should return self minus other fixnum" do
    example do
      p [9_237_212.5280 - 5_280, 2_560_496.1691 - 0xfffffffff, 5.5 - 5.5]
    end.should == '[9231932.528, -68716916238.8309, 0.0]'
  end
  
  specify "-@ should negate self" do
    example do
      p [2.221.send(:-@), -2.01, -2_455_999_221.5512, --5.5, -8.551.send(:-@)]
    end.should == '[-2.221, -2.01, -2455999221.5512, 5.5, 8.551]'
  end
  
end
