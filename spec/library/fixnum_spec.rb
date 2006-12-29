require File.dirname(__FILE__) + '/../spec_helper'

# %, &, *, **, +, -, -@, /, <, <<, <=, <=>, ==, >, >=, >>, [], ^,
# abs, div, divmod, id2name, modulo, power!, quo, quo, rdiv, rpower,
# size, to_f, to_s, to_sym, zero?, |, ~

context "Fixnum" do
  
  specify "% should return self modulo other" do
    example do
      p [451 % 2, 93 % 3]
    end.should == '[1, 0]'
  end

  specify "& should return self bitwise AND other fixnum" do
    example do
      p [2010 & 5, 65535 & 1, 256 & 16]
    end.should == '[0, 1, 0]'
  end
  
  specify "& should return self bitwise AND other bignum" do
    example do
      p [0xffff & 0xffffffff]
    end.should == '[65535]'
  end
  
  specify "* should return self multiplied by other fixnum" do
    example do
      p [4923 * 2, 5_345_291 * 800]
    end.should == '[9846, 4276232800]'
  end
  
  specify "* should return self multiplied by other bignum" do
    example do
      p [256 * 0xffffffff]
    end.should == '[1099511627520]'
  end
  
  specify "* should return self multiplied by other float" do
    example do
      p [6712 * 0.25]
    end.should == '[1678.0]'
  end
  
  specify "** should return self raise to the other power" do
    example do
      p [2 ** 3, 5 ** -1, 9 ** 0.5]
    end.should == '[8, 0.2, 3.0]'
  end
  
  specify "+ should return self plus other fixnum" do
    example do
      p [491 + 2, 90210 + 10, 6000 + 144000]
    end.should == '[493, 90220, 150000]'
  end
  
  specify "+ should return self plus other bignum" do
    example do
      p [9 + 0xffffffff]
    end.should == '[4294967304]'
  end
  
  specify "+ should return self plus other float" do
    example do
      p [1001 + 5.219]
    end.should == '[1006.219]'
  end
  
  specify "- should return self minus other fixnum" do
    example do
      p [5 - 10, 259 - 3, 9_237_212 - 5_280]
    end.should == '[-5, 256, 9231932]'
  end
  
  specify "- should return self minus other bignum" do
    example do
      p [4 - 0xffffffff, 2_560_496 - 0xfffffffff]
    end.should == '[-4294967291, -68716916239]'
  end
  
  specify "- should return self minus other float" do
    example do
      p [5 - 5.0, 781 - 0.5, 9 - 200.25]
    end.should == '[0.0, 780.5, -191.25]'
  end
end
