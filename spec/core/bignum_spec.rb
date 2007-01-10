require File.dirname(__FILE__) + '/../spec_helper'

# %, &, *, **, **, +, -, -@, /, <<, <=>, ==, >>, [], ^, abs, coerce,
# div, divmod, eql?, hash, modulo, power!, quo, quo, rdiv, remainder,
# rpower, size, to_f, to_s, |, ~

context "Bignum instance method" do
  # the smallest bignum in MRI is a different value
  # than the smallest bignum in Rubinius, however,
  # rubinius(smallest_bignum) <= MRI(smallest_bignum)
  # so these specs are written such that they should
  # pass on either target.
  setup do
    @src = <<-CODE
    module B
      def self.sbm(plus=0)
        (0x3fffffff + 1) + plus
      end
    end
    CODE
  end

  specify "% should return self modulo other" do
    example(@src) do
      @a = B.sbm(2_222)
      p [@a % 5, @a % 2.22, @a % B.sbm]
    end.should == '[1, 0.419999905491539, 2222]'
  end
  
  specify "& should return self bitwise AND other" do
    example(@src) do
      @a = B.sbm(5)
      p [@a & 3.4, @a & 52, @a & B.sbm(9921)]
    end.should == '[1, 4, 1073741825]'
  end
  
  specify "* should return self multiplied by other" do
    example(@src) do
      @a = B.sbm(772)
      p [@a * 98.6, @a * 10, @a * (@a - 40)]
    end.should == '[105871019965.6, 10737425960, 1152923119515115376]'
  end
  
  specify "** should return self raised to other power" do
    example(@src) do
      @a = B.sbm(47)
      p [@a ** 5.2, @a ** 4]
    end.should == '[9.13438731244363e+46, 1329228228517658539377366716859970881]'
  end
  
  specify "+ should return self plus other" do
    example(@src) do
      @a = B.sbm(76)
      p [@a + 4, @a + 4.2, @a + B.sbm(3)]
    end.should == '[1073741904, 1073741904.2, 2147483727]'
  end
  
  specify "- should return self minus other" do
    example(@src) do
      @a = B.sbm(314)
      p [@a - 9, @a - 12.57, @a - B.sbm(42)]
    end.should == '[1073742129, 1073742125.43, 272]'
  end
  
  specify "-@ should negate self" do
    example(@src) do
      p [B.sbm.send(:-@), B.sbm(921).send(:-@)]
    end.should == '[-1073741824, -1073742745]'
  end
  
  specify "/ should return self divided by other" do
    example(@src) do
      @a = B.sbm(88)
      p [@a / 4, @a / 16.2, @a / B.sbm(2)]
    end.should == '[268435478, 66280364.9382716, 1]'
  end
  
  specify "< should return true if self is less than other" do
    example(@src) do
      @a = B.sbm(32)
      p [@a < 2, @a < 456_912.410, @a < B.sbm(42)]
    end.should == '[false, false, true]'
  end
  
  specify "<= should return true if self is less than or equal to other" do
    example(@src) do
      @a = B.sbm(39)
      p [@a <= @a, @a <= 4, @a <= 45.8]
    end.should == '[true, false, false]'
  end
  
  specify "<=> should return -1, 0, 1 when self is less than, equal, or greater than other" do
    example(@src) do
      @a = B.sbm(96)
      p [@a <=> B.sbm(196), @a <=> @a, @a <=> 4.5]
    end.should == '[-1, 0, 1]'
  end
  
  specify "== should true if self has the same value as other" do
    example(@src) do
      @a = B.sbm(67)
      p [@a == @a, @a == 5.4, @a == 121]
    end.should == '[true, false, false]'
  end
  
  specify "> should return true if self is greater than other" do
    example(@src) do
      @a = B.sbm(732)
      p [@a > (@a + 500), @a > 14.6, @a > (B.sbm - 1)]
    end.should == '[false, true, true]'
  end
  
  specify ">= should return true if self is greater than or equal to other" do
    example(@src) do
      @a = B.sbm(14)
      p [@a >= @a, @a >= (@a -2), @a >= 5664.2, @a >= 4]
    end.should == '[true, true, false, false]'
  end

  specify "abs should return the absolute value" do
    example(@src) do
      p []
    end.should == ''
  end
  
  specify "<< should return self shifted left other bits" do
    example(@src) do
      p []
    end.should == ''
  end
  
  specify ">> should return self shifted right other bits" do
    example(@src) do
      p []
    end.should == ''
  end
  
  specify "[] should return the nth bit in the binary representation of self" do
    example(@src) do
      p []
    end.should == ''
  end
  
  specify "^ should return self bitwise EXCLUSIVE OR other" do
    example(@src) do
      p []
    end.should == ''
  end
  
  specify "coerce should return [other, self] both as Bignum if other is an Integer" do
    example(@src) do
      p []
    end.should == ''
  end
  
  specify "div should be a synonym for /" do
    example(@src) do
      @a = B.sbm(41622)
      p [@a.div(4), @a.div(16.2), @a.div(B.sbm(2))]
    end.should == '[268445861, 66282928.7654321, 1]'
  end
  
  specify "divmod should return an [quotient, modulus] from dividing self by other" do
    example(@src) do
      p []
    end.should == ''
  end
  
  specify "eql? should return true if other is a Bignum with the same value" do
    example(@src) do
      p []
    end.should == ''
  end
  
  specify "hash should be provided" do
    example(@src) do
      p []
    end.should == ''
  end
  
  specify "modulo should be a synonym for %" do
    example(@src) do
      @a = B.sbm(2_222)
      p [@a.modulo(5), @a.modulo(2.22), @a.modulo(B.sbm)]
    end.should == '[1, 0.419999905491539, 2222]'
  end
  
  specify "quo should return the floating-point result of self divided by other" do
    example(@src) do
      p []
    end.should == ''
  end
  
  specify "rdiv should should be a synonym for quo" do
    example(@src) do
      p []
    end.should == ''
  end
  
  specify "remainder should return the remainder of dividing self by other" do
    example(@src) do
      p []
    end.should == ''
  end
  
  specify "size should be provided" do
    example(@src) do
      p B.sbm.respond_to?(:size)
    end.should == 'true'
  end
  
  specify "to_f should return self converted to Float" do
    example(@src) do
      p []
    end.should == ''
  end
  
  specify "to_s should return a string representation of self" do
    example(@src) do
      p []
    end.should == ''
  end
  
  specify "| should return self bitwise OR other" do
    example(@src) do
      p []
    end.should == ''
  end
  
  specify "~ should return self bitwise inverted" do
    example(@src) do
      p []
    end.should == ''
  end  
end
