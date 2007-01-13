require File.dirname(__FILE__) + '/../spec_helper'

# %, &, *, **, +, -, -@, /, <, <<, <=, <=>, ==, >, >=, >>, [], ^,
# abs, div, divmod, id2name, modulo, power!, quo, rdiv, rpower,
# size, to_f, to_s, to_sym, zero?, |, ~

context "Fixnum instance method" do
  
  specify "% should return self modulo other" do
    example do
      [451 % 2, 93 % 3.2, 120 % -4.5]
    end.should == [1, 0.199999999999995, -1.5]
  end

  specify "& should return self bitwise AND other" do
    example do
      [2010 & 5, 65535 & 1, 256 & 16, 0xffff & 0xffffffff]
    end.should == [0, 1, 0, 65535]
  end
  
  specify "* should return self multiplied by other" do
    example do
      [4923 * 2, 5_345_291 * 800, 256 * 0xffffffff, 6712 * 0.25]
    end.should == [9846, 4276232800, 1099511627520, 1678.0]
  end
  
  specify "** should return self raise to the other power" do
    example do
      [2 ** 3, 5 ** -1, 9 ** 0.5]
    end.should == [8, 0.2, 3.0]
  end
  
  specify "+ should return self plus other" do
    example do
      [491 + 2, 90210 + 10, 9 + 0xffffffff, 1001 + 5.219]
    end.should == [493, 90220, 4294967304, 1006.219]
  end
  
  specify "- should return self minus other fixnum" do
    example do
      [5 - 10, 9_237_212 - 5_280, 2_560_496 - 0xfffffffff, 781 - 0.5]
    end.should == [-5, 9231932, -68716916239, 780.5]
  end
  
  specify "-@ should negate self" do
    example do
      [2.send(:-@), -2, -0xfffffff, --5, -8.send(:-@)]
    end.should == [-2, -2, -268435455, 5, 8]
  end

  specify "/ should return self divided by other" do
    example do
      [2 / 2, -1 / 50.4, 1 / 0xffffffff]
    end.should == [1, -0.0198412698412698, 0]
  end
  
  specify "< should return true if self is less than other" do
    example do
      [-1 < 0, 3 < -5.2, 9 < 0xffffffff]
    end.should == [true, false, true]
  end
  
  specify "<= should return true if self is less than or equal to other" do
    example do
      [2 <= 2, 7 <= 5.4, 11 <= 0xffffffff]
    end.should == [true, false, true]
  end
  
  specify "<=> should return -1, 0, 1 when self is less than, equal, or greater than other" do
    example do
      [-1 <=> 0xffffffff, 954 <=> 954.0, 496 <=> 5]
    end.should == [-1, 0, 1]
  end
  
  specify "== should true if self has the same value as other" do
    example do
      [1 == 1.0, 9 == 5, 2 == 0xffffffff]
    end.should == [true, false, false]
  end
  
  specify "> should return true if self is greater than other" do
    example do
      [-500 > -600, 13 > 2.178, 11 > 0xffffffff]
    end.should == [true, true, false]
  end
  
  specify ">= should return true if self is greater than or equal to other" do
    example do
      [-50 >= -50, 14 >= 2.5, 900 >= 0xffffffff]
    end.should == [true, true, false]
  end

  specify "abs should return the absolute value" do
    example do
      [0.abs, -2.abs, 5.abs]
    end.should == [0, 2, 5]
  end
  
  specify "<< should return self shifted left other bits" do
    example do
      [7 << 2, 9 << 4.2, 6 << 0xff]
    end.should == [28, 144, 347376267711948586270712955026063723559809953996921692118372752023739388919808]
  end
  
  specify ">> should return self shifted right other bits" do
    example do
      [7 >> 1.5, 0xfff >> 3, 9_245_278 >> 1]
    end.should == [3, 511, 4622639]
  end
  
  specify "[] should return the nth bit in the binary representation of self" do
    example do
      [2[3], 15[1.3], 3[0xffffffff]]
    end.should == [0, 1, 0]
  end
  
  specify "^ should return self bitwise EXCLUSIVE OR other" do
    example do
      [3 ^ 5, -2 ^ -255, -7 ^ 15.2, 5 ^ 0xffffffff]
    end.should == [6, 255, -10, 4294967290]
  end
  
  specify "div should return self divided by other as an Integer" do
    example do
      [2.div(2), -1.div(50.4), 1.div(0xffffffff)]
    end.should == [1, -1, 0]
  end
  
  specify "divmod should return an [quotient, modulus] from dividing self by other" do
    example do
      [1.divmod(2.0), -5.divmod(3), 200.divmod(0xffffffff)]
    end.should == [[0, 1.0], [-2, 1], [0, 200]]
  end
  
  specify "id2name should return the string name of the object whose symbol ID is self" do
    example do
      @a = :@sym
      @b = :@ruby
      @c = :@rubinius
      [@a.to_i.id2name, @b.to_i.id2name, @c.to_i.id2name]
    end.should == ["@sym", "@ruby", "@rubinius"]
  end
  
  specify "modulo should be a synonym for %" do
    example do
      [451.modulo(2), 93.modulo(3.2), 120.modulo(-4.5)]
    end.should == [1, 0.199999999999995, -1.5]
  end
  
  specify "quo should return the floating-point result of self divided by other" do
    example do
      [2.quo(2.5), 5.quo(2), 45.quo(0xffffffff)]
    end.should == [0.8, 2.5, 1.04773789668636e-08]
  end
  
  specify "size should be provided" do
    example do
      1.respond_to?(:size)
    end.should == true
  end
  
  specify "to_f should return self converted to Float" do
    example do
      [0.to_f, -500.to_f, 9_641_278.to_f]
    end.should == [0.0, -500.0, 9641278.0]
  end
  
  specify "to_s should return a string representation of self" do
    example do
      [0xff, 3, 0, -9_002]
    end.should == [255, 3, 0, -9002]
  end
  
  specify "to_sym should return the symbol whose integer value is self" do
    example do
      @a = :fred.to_i
      @b = :wilma.to_i
      @c = :bambam.to_i
      [@a.to_sym, @b.to_sym, @c.to_sym]
    end.should == [:fred, :wilma, :bambam]
  end
  
  specify "zero? should return true if self is 0" do
    example do
      [0.zero?, -1.zero?, 1.zero?]
    end.should == [true, false, false]
  end
  
  specify "| should return self bitwise OR other" do
    example do
      [1 | 0, 5 | 4, 5 | 6, 248 | 4096]
    end.should == [1, 5, 7, 4344]
  end
  
  specify "~ should return self bitwise inverted" do
    example do
      [~0, ~1221, ~-599, ~-2]
    end.should == [-1, -1222, 598, 1]
  end  
end
