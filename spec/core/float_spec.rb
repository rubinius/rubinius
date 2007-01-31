require File.dirname(__FILE__) + '/../spec_helper'

# %, *, **, +, -, -@, /, <, <=, <=>, ==, >, >=, abs, ceil, coerce,
# divmod, eql?, finite?, floor, hash, infinite?, modulo, nan?, round,
# to_f, to_i, to_int, to_s, truncate, zero?

context "Float" do
  specify "% should return self modulo other" do
    example do
      [6543.21 % 137, 5667.19 % 0xffffffff, 6543.21 % 137.24].inspect
    end.should == '[104.21, 5667.19, 92.9299999999996]'
  end
  
  specify "* should return self multiplied by other" do
    example do
      [4923.98221 * 2, (256.4096 * 0xffffffff).to_s, 6712.5 * 0.25]
    end.should == [9847.96442, "1101270846124.03", 1678.125]
  end
  
  specify "** should return self raise to the other power" do
    example do
      [2.3 ** 3, 5.2 ** -1, 9.5 ** 0.5, 9.5 ** 0xffffffff].inspect
    end.should == '[12.167, 0.192307692307692, 3.08220700148449, Infinity]'
  end
  
  specify "+ should return self plus other" do
    example do
      [491.213 + 2, 9.99 + 0xffffffff, 1001.99 + 5.219].inspect
    end.should == '[493.213, 4294967304.99, 1007.209]'
  end

  specify "- should return self minus other" do
    example do
      [9_237_212.5280 - 5_280, 2_560_496.1691 - 0xfffffffff, 5.5 - 5.5]
    end.should == [9231932.528, -68716916238.8309, 0.0]
  end
  
  specify "-@ should negate self" do
    example do
      [2.221.send(:-@), -2.01, -2_455_999_221.5512, --5.5, -8.551.send(:-@)]
    end.should == [-2.221, -2.01, -2455999221.5512, 5.5, 8.551]
  end

  specify "/ should return self divided by other" do
    example do
      [(5.75 / -2).to_s, (451.0 / 9.3).to_s, (91.1 / -0xffffffff).to_s]
    end.should == ["-2.875", "48.494623655914", "-2.12108716418061e-08"]
  end
  
  specify "< should return true if self is less than other" do
    example do
      [71.3 < 91.8, 192.6 < -500,  -0.12 < 0x4fffffff]
    end.should == [true, false, true]
  end
  
  specify "<= should return true if self is less than or equal to other" do
    example do
      [2.0 <= 3.14159, -2.7183 <= -24, 0.0 <= 0.0, 9_235.9 <= 0xffffffff]
    end.should == [true, false, true, true]
  end
  
  specify "<=> should return -1, 0, 1 when self is less than, equal, or greater than other" do
    example do
      [1.5 <=> 5, 2.45 <=> 2.45, (0xffffffff*1.1) <=> 0xffffffff]
    end.should == [-1, 0, 1]
  end
  
  specify "== should true if self has the same value as other" do
    example do
      [1.0 == 1, 2.71828 == 1.428, -4.2 == 4.2]
    end.should == [true, false, false]
  end
  
  specify "> should return true if self is greater than other" do
    example do
      [1.5 > 1, 2.5 > 3, 45.91 > 0xffffffff]
    end.should == [true, false, false]
  end
  
  specify ">= should return true if self is greater than or equal to other" do
    example do
      [5.2 >= 5.2, 9.71 >= 1, 5.55382 >= 0xfabdafbafcab]
    end.should == [true, true, false]
  end

  specify "abs should return the absolute value" do
    example do
      [-99.1.abs, 4.5.abs, 0.0.abs]
    end.should == [99.1, 4.5, 0.0]
  end
  
  specify "ceil should return the smallest Integer greater than or equal to self" do
    example do
      [-0.0.ceil, -1.0.ceil, 3.14159.ceil]
    end.should == [0, -1, 4]
  end
  
  specify "coerce should [other, self] both as Floats" do
    example do
      [1.2.coerce(1), -0.0.coerce(0xffffffff), 5.28.coerce(1.0)]
    end.should == [[1.0, 1.2], [4294967295.0, -0.0], [1.0, 5.28]]
  end
  
  specify "divmod should return an [quotient, modulus] from dividing self by other" do
    example do
      [3.14.divmod(2).inspect, 2.8284.divmod(3.1415).inspect, -1.0.divmod(0xffffffff).inspect]
    end.should == ["[1, 1.14]", "[0, 2.8284]", "[-1, 4294967294.0]"]
  end
  
  specify "eql? should return true if other is a Float equal to self" do
    example do
      [1.0.eql?(1), 0.0.eql?(0.0)]
    end.should == [false, true]
  end
  
  specify "finite? should return true if a valid IEEE floating-point number" do
    example do
      [(1.5**0xffffffff).finite?, 3.14159.finite?, (-1.0/0.0).finite?]
    end.should == [false, true, false]
  end
  
  specify "floor should return the largest Integer less than or equal to self" do
    example do
      [-1.0.floor, 0.2.floor, 5.9.floor]
    end.should == [-1, 0, 5]
  end
  
  specify "hash should be provided" do
    example do
      0.0.respond_to?(:hash)
    end.should == true
  end
  
  specify "infinite? should return nil, -1, +1 when self is finite, -Infinity, +Infinity" do
    example do
      [1.0.infinite?, (1.0/0.0).infinite?, (1.0/-0.0).infinite?]
    end.should == [nil, 1, -1]
  end
  
  specify "modulo should be a synonym for %" do
    example do
      [6543.21.modulo(137), 5667.19.modulo(0xffffffff), 6543.21.modulo(137.24)].inspect
    end.should == '[104.21, 5667.19, 92.9299999999996]'
  end
  
  specify "nan? should return true if self is not a valid IEEE floating-point number" do
    example do
      [0.0.nan?, -1.5.nan?, (0.0/0.0).nan?]
    end.should == [false, false, true]
  end
  
  specify "round should return the nearest Integer" do
    example do
      [5.5.round, 0.4.round, -2.8.round, 0.0.round]
    end.should == [6, 0, -3, 0]
  end
  
  specify "to_f should return self" do
    example do
      [-500.3.to_f, 267.51.to_f, 1.1412.to_f]
    end.should == [-500.3, 267.51, 1.1412]
  end
  
  specify "to_i should return self truncated to an Integer" do
    example do
      [899.2.to_i, -1.122256e-45.to_i, 5_213_451.9201.to_i]
    end.should == [899, 0, 5213451]
  end
  
  specify "to_int should be a synonym for to_i" do
    example do
      [899.2.to_int, -1.122256e-45.to_int, 5_213_451.9201.to_int]
    end.should == [899, 0, 5213451]
  end
  
  specify "to_s should return a string representation of self, possibly Nan, -Infinity, +Infinity" do
    example do
      [0.551e7.to_s, -3.14159.to_s, 0.0.to_s]
    end.should == ["5510000.0", "-3.14159", "0.0"]
  end
  
  specify "truncate should be a synonym for to_i" do
    example do
      [899.2.truncate, -1.122256e-45.truncate, 5_213_451.9201.truncate]
    end.should == [899, 0, 5213451]
  end
  
  specify "zero? should return true if self is 0.0" do
    example do
      [0.0.zero?, 1.0.zero?, -1.0.zero?]
    end.should == [true, false, false]
  end
  
  specify "coerce should return [Float, Float]" do
    example do
      [1.0.coerce(1), 1.0.coerce(0xffffffff), 1.0.coerce("2.5"), 1.0.coerce(3.14)]
    end.should == [[1.0, 1.0], [4294967295.0, 1.0], [2.5, 1.0], [3.14, 1.0]]
  end
end
