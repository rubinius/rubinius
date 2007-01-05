require File.dirname(__FILE__) + '/../spec_helper'

# %, &, *, **, +, -, -@, /, <, <<, <=, <=>, ==, >, >=, >>, [], ^,
# abs, div, divmod, id2name, modulo, power!, quo, rdiv, rpower,
# size, to_f, to_s, to_sym, zero?, |, ~

context "Fixnum" do
  
  specify "% should return self modulo other" do
    example do
      p [451 % 2, 93 % 3.2, 120 % -4.5]
    end.should == '[1, 0.199999999999995, -1.5]'
  end

  specify "& should return self bitwise AND other" do
    example do
      p [2010 & 5, 65535 & 1, 256 & 16, 0xffff & 0xffffffff]
    end.should == '[0, 1, 0, 65535]'
  end
  
  specify "* should return self multiplied by other" do
    example do
      p [4923 * 2, 5_345_291 * 800, 256 * 0xffffffff, 6712 * 0.25]
    end.should == '[9846, 4276232800, 1099511627520, 1678.0]'
  end
  
  specify "** should return self raise to the other power" do
    example do
      p [2 ** 3, 5 ** -1, 9 ** 0.5]
    end.should == '[8, 0.2, 3.0]'
  end
  
  specify "+ should return self plus other" do
    example do
      p [491 + 2, 90210 + 10, 9 + 0xffffffff, 1001 + 5.219]
    end.should == '[493, 90220, 4294967304, 1006.219]'
  end
  
  specify "- should return self minus other fixnum" do
    example do
      p [5 - 10, 9_237_212 - 5_280, 2_560_496 - 0xfffffffff, 781 - 0.5]
    end.should == '[-5, 9231932, -68716916239, 780.5]'
  end
  
  specify "-@ should negate self" do
    example do
      p [2.send(:-@), -2, -0xfffffff, --5, -8.send(:-@)]
    end.should == '[-2, -2, -268435455, 5, 8]'
  end
end
