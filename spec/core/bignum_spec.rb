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
        (0x40000000) + plus
      end
    end
    CODE
  end

  specify "% should return self modulo other" do
    example(@src) do
      @a = B.sbm(2_222)
      [@a % 5, @a % 2.22, @a % B.sbm].inspect
    end.should == '[1, 0.419999905491539, 2222]'
  end

  specify "% should raise ZeroDivisionError if other is zero and not a Float" do
    example do
      @a = B.sbm
      try(ZeroDivisionError) { @a % 0 }
    end.should == true
  end
  
  specify "% should NOT raise ZeroDivisionError if other is zero and is a Float" do
    example do
      @a = B.sbm(5_221)
      @b = B.sbm(25)
      [@a % 0.0, @b % -0.0].inspect
    end.should == '[NaN, NaN]'
  end
  
  specify "& should return self bitwise AND other" do
    example(@src) do
      @a = B.sbm(5)
      [@a & 3.4, @a & 52, @a & B.sbm(9921)]
    end.should == [1, 4, 1073741825]
  end
  
  specify "* should return self multiplied by other" do
    example(@src) do
      @a = B.sbm(772)
      [@a * 98.6, @a * 10, @a * (@a - 40)].inspect
    end.should == '[105871019965.6, 10737425960, 1152923119515115376]'
  end
  
  specify "** should return self raised to other power" do
    example(@src) do
      @a = B.sbm(47)
      [@a ** 5.2, @a ** 4]
    end.should == [9.13438731244363e+46, 1329228228517658539377366716859970881]
  end
  
  specify "+ should return self plus other" do
    example(@src) do
      @a = B.sbm(76)
      [@a + 4, @a + 4.2, @a + B.sbm(3)]
    end.should == [1073741904, 1073741904.2, 2147483727]
  end
  
  specify "- should return self minus other" do
    example(@src) do
      @a = B.sbm(314)
      [@a - 9, @a - 12.57, @a - B.sbm(42)]
    end.should == [1073742129, 1073742125.43, 272]
  end
  
  specify "-@ should negate self" do
    example(@src) do
      [B.sbm.send(:-@), B.sbm(921).send(:-@)]
    end.should == [-1073741824, -1073742745]
  end
  
  specify "/ should return self divided by other" do
    example(@src) do
      @a = B.sbm(88)
      [@a / 4, @a / 16.2, @a / B.sbm(2)].inspect
    end.should == '[268435478, 66280364.9382716, 1]'
  end
  
  specify "/ should raise ZeroDivisionError if other is zero and not a Float" do
    example do
      @a = B.sbm
      try(ZeroDivisionError) { @a / 0 }
    end.should == true
  end
  
  specify "/ should NOT raise ZeroDivisionError if other is zero and is a Float" do
    example do
      @a = B.sbm(5_221)
      @b = B.sbm(25)
      [@a / 0.0, @b / -0.0].inspect
    end.should == '[Infinity, -Infinity]'
  end
  
  specify "< should return true if self is less than other" do
    example(@src) do
      @a = B.sbm(32)
      [@a < 2, @a < 456_912.410, @a < B.sbm(42)]
    end.should == [false, false, true]
  end
  
  specify "<= should return true if self is less than or equal to other" do
    example(@src) do
      @a = B.sbm(39)
      [@a <= @a, @a <= 4, @a <= 45.8]
    end.should == [true, false, false]
  end
  
  specify "<=> should return -1, 0, 1 when self is less than, equal, or greater than other" do
    example(@src) do
      @a = B.sbm(96)
      [@a <=> B.sbm(196), @a <=> @a, @a <=> 4.5]
    end.should == [-1, 0, 1]
  end
  
  specify "== should true if self has the same value as other" do
    example(@src) do
      @a = B.sbm(67)
      [@a == @a, @a == 5.4, @a == 121]
    end.should == [true, false, false]
  end
  
  specify "> should return true if self is greater than other" do
    example(@src) do
      @a = B.sbm(732)
      [@a > (@a + 500), @a > 14.6, @a > (B.sbm - 1)]
    end.should == [false, true, true]
  end
  
  specify ">= should return true if self is greater than or equal to other" do
    example(@src) do
      @a = B.sbm(14)
      [@a >= @a, @a >= (@a + 2), @a >= 5664.2, @a >= 4]
    end.should == [true, false, true, true]
  end

  specify "abs should return the absolute value" do
    example(@src) do
      [B.sbm(39).abs, (-B.sbm(18)).abs]
    end.should == [1073741863, 1073741842]
  end
  
  specify "<< should return self shifted left other bits" do
    example(@src) do
      @a = B.sbm(9)
      [@a << 4, @a << 1.5, @a << 9]
    end.should == [17179869328, 2147483666, 549755818496]
  end
  
  specify ">> should return self shifted right other bits" do
    example(@src) do
      @a = B.sbm(90812)
      [@a >> 3.45, @a >> 2, @a >> 21]
    end.should == [134229079, 268458159, 512]
  end
  
  specify "[] should return the nth bit in the binary representation of self" do
    example(@src) do
      @a = B.sbm(4996)
      [@a[2], @a[9.2], @a[21]]
    end.should == [1, 1, 0]
  end
  
  specify "^ should return self bitwise EXCLUSIVE OR other" do
    example(@src) do
      @a = B.sbm(18)
      [@a ^ 2, @a ^ @a, @a ^ 14.5]
    end.should == [1073741840, 0, 1073741852]
  end
  
  specify "coerce should return [other, self] both as Bignum if other is an Integer" do
    example(@src) do
      @a = B.sbm
      [@a.coerce(2), @a.coerce(B.sbm(701))]
    end.should == [[2, 1073741824], [1073742525, 1073741824]]
  end
  
  specify "div should be a synonym for /" do
    example(@src) do
      @a = B.sbm(41622)
      [@a.div(4), @a.div(16.2), @a.div(B.sbm(2))].inspect
    end.should == '[268445861, 66282928.7654321, 1]'
  end
  
  specify "div should raise ZeroDivisionError if other is zero and not a Float" do
    example do
      @a = B.sbm
      try(ZeroDivisionError) { @a.div(0) }
    end.should == true
  end
  
  specify "div should NOT raise ZeroDivisionError if other is zero and is a Float" do
    example do
      @a = B.sbm(5_221)
      @b = B.sbm(25)
      [@a.div(0.0), @b.div(-0.0)].inspect
    end.should == '[Infinity, -Infinity]'
  end
  
  specify "divmod should return an [quotient, modulus] from dividing self by other" do
    example(@src) do
      @a = B.sbm(55)
      [@a.divmod(5), @a.divmod(15.2), @a.divmod(@a + 9)].inspect
    end.should == '[[214748375, 4], [70640913, 1.40000005019339], [0, 1073741879]]'
  end
  
  specify "divmod should raise ZeroDivisionError if other is zero and not a Float" do
    example do
      @a = B.sbm(2)
      try(ZeroDivisionError) { @a.divmod(0) }
    end.should == true
  end
  
  specify "divmod should raise FloatDomainError if other is zero and is a Float" do
    example do
      @a = B.sbm(9)
      try(FloatDomainError) { @a.divmod(0.0) }
    end
  end

  specify "eql? should return true if other is a Bignum with the same value" do
    example(@src) do
      @a = B.sbm(13)
      [@a.eql?(B.sbm(13)), @a.eql?(2), @a.eql?(3.14)]
    end.should == [true, false, false]
  end
  
  specify "hash should be provided" do
    example(@src) do
      B.sbm.respond_to?(:hash)
    end.should == true
  end
  
  specify "modulo should be a synonym for %" do
    example(@src) do
      @a = B.sbm(2_222)
      [@a.modulo(5), @a.modulo(2.22), @a.modulo(B.sbm)].inspect
    end.should == '[1, 0.419999905491539, 2222]'
  end
  
  specify "% should raise ZeroDivisionError if other is zero and not a Float" do
    example do
      @a = B.sbm
      try(ZeroDivisionError) { @a.modulo(0) }
    end.should == true
  end
  
  specify "modulo should NOT raise ZeroDivisionError if other is zero and is a Float" do
    example do
      @a = B.sbm(5_221)
      @b = B.sbm(25)
      [@a.modulo(0.0), @b.modulo(-0.0)].inspect
    end.should == '[NaN, NaN]'
  end
  
  specify "quo should return the floating-point result of self divided by other" do
    example(@src) do
      @a = B.sbm(3)
      [@a.quo(2.5).to_f, @a.quo(13).to_f, @a.quo(B.sbm).to_f].inspect
    end.should == '[429496730.8, 82595525.1538462, 1.00000000279397]'
  end

  specify "quo should NOT raise an exception when other is zero" do
    example do
      # @a.quo(0) should also not raise (i.e works in irb and from a file),
      # but fails here.
      @a = B.sbm(91)
      @b = B.sbm(28)
      [@a.quo(0.0), @b.quo(-0.0)].inspect
    end.should == '[Infinity, -Infinity]'
  end
  
  specify "remainder should return the remainder of dividing self by other" do
    example(@src) do
      @a = B.sbm(79)
      [@a.remainder(2), @a.remainder(97.345), @a.remainder(B.sbm)].inspect
    end.should == '[1, 75.16000001254, 79]'
  end
  
  specify "remainder should raise ZeroDivisionError if other is zero and not a Float" do
    example do
      try(ZeroDivisionError) { B.sbm(66).remainder(0) }
    end.should == true
  end
  
  specify "remainder should NOT raise ZeroDivisionError if other is zero and is a Float" do
    example do
      @a = B.sbm(7)
      @b = B.sbm(32)
      [@a.remainder(0.0), @b.remainder(-0.0)].inspect
    end.should == '[NaN, NaN]'
  end    
  
  specify "size should be provided" do
    example(@src) do
      B.sbm.respond_to?(:size)
    end.should == true
  end
  
  specify "to_f should return self converted to Float" do
    example(@src) do
      [B.sbm(2).to_f, (-B.sbm(99)).to_f, B.sbm(14).to_f]
    end.should == [1073741826.0, -1073741923.0, 1073741838.0]
  end
  
  specify "to_s should return a string representation of self" do
    example(@src) do
      [B.sbm(9).to_s, B.sbm.to_s, (-B.sbm(675)).to_s]
    end.should == ["1073741833", "1073741824", "-1073742499"]
  end
  
  specify "| should return self bitwise OR other" do
    example(@src) do
      @a = B.sbm(11)
      [@a | 2, @a | 9.9, @a | B.sbm]
    end.should == [1073741835, 1073741835, 1073741835]
  end
  
  specify "~ should return self bitwise inverted" do
    example(@src) do
      @a = B.sbm(48)
      [~B.sbm(48), ~(-B.sbm(21)), ~B.sbm(1)]
    end.should == [-1073741873, 1073741844, -1073741826]
  end  

  specify "coerce should return [Bignum, Bignum] if other is a Bignum" do
    example do
      @a = 0xffffffff.coerce(0xffffffff)
      [@a, @a.collect { |i| i.class }]
    end.should == [[4294967295, 4294967295], [Bignum, Bignum]]
  end
end
