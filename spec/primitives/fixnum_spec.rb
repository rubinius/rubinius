require File.dirname(__FILE__) + '/../primitives_spec_helper'

context "The add primitive sent two fixnums" do
  specify "should return the sum of its arguments" do
    run_primitive(:add, 500, 500).should == 1000
    run_primitive(:add, 0, 0).should == 0
    run_primitive(:add, -11, -11).should == -22
  end
end

context "The add primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:add, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:add, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:add, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:add, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:add, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:add, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:add, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The add primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:add, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:add, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:add, 1).should_raise(ArgumentError) # 0 args
  end
end

context "The compare primitive sent two fixnums" do
  specify "should return 1 if x > y" do
    run_primitive(:compare, 30, 20).should == 1
    run_primitive(:compare, 10, -5).should == 1
    run_primitive(:compare, -10, -500).should == 1
  end

  specify "should return 0 if x == y" do
    run_primitive(:compare, 500, 500).should == 0
    run_primitive(:compare, 0, 0).should == 0
    run_primitive(:compare, -11, -11).should == 0
  end

  specify "should return -1 if x < y" do
    run_primitive(:compare, 3, 500).should == -1
    run_primitive(:compare, -8, 3).should == -1
    run_primitive(:compare, -1000, -20).should == -1
  end

end

context "The compare primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:compare, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:compare, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:compare, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:compare, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:compare, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:compare, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:compare, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The compare primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:compare, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:compare, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:compare, 1).should_raise(ArgumentError) # 0 args
  end
end

context "The equal primitive sent two fixnums" do
  specify "should return true if x == y" do
    run_primitive(:equal, 500, 500).should == true
    run_primitive(:equal, 0, 0).should == true
    run_primitive(:equal, -11, -11).should == true
  end

  specify "should return false if x != y" do
    run_primitive(:equal, 3, 500).should == false
    run_primitive(:equal, -8, 3).should == false
    run_primitive(:equal, -1000, -20).should == false
  end
end

context "The equal primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:equal, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:equal, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:equal, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:equal, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:equal, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:equal, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:equal, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The equal primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:equal, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:equal, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:equal, 1).should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_and primitive sent two fixnums" do
  specify "should return the bitwise and of its' arguments" do
    run_primitive(:fixnum_and, 500, 500).should == 500
    run_primitive(:fixnum_and, 0, 0).should == 0
    run_primitive(:fixnum_and, -11, -11).should == -11
    run_primitive(:fixnum_and, 5, 1).should == 1
    run_primitive(:fixnum_and, 2, 1).should == 0
    run_primitive(:fixnum_and, 15, 1).should == 1
    run_primitive(:fixnum_and, 15, 7).should == 7
    run_primitive(:fixnum_and, -1, 1).should == 1
    run_primitive(:fixnum_and, -2, 1).should == 0
    run_primitive(:fixnum_and, -8, 7).should == 0
    run_primitive(:fixnum_and, -16, 16).should == 16
  end
end

context "The fixnum_and primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:fixnum_and, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_and, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:fixnum_and, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_and, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:fixnum_and, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_and, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_and, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_and primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:fixnum_and, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_and, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:fixnum_and, 1).should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_divmod primitive sent two fixnums" do
  specify "should return the quotient and remainder of its arguments" do
    run_primitive(:fixnum_divmod, 5, 5).should == [1, 0]
    run_primitive(:fixnum_divmod, -11, -11).should == [1, 0]
    run_primitive(:fixnum_divmod, -20, 5).should == [-4, 0]
    run_primitive(:fixnum_divmod, 10, 1).should == [10, 0]
    run_primitive(:fixnum_divmod, 3, 320).should == [0, 3]
    run_primitive(:fixnum_divmod, 10, 50).should == [0, 10]
    run_primitive(:fixnum_divmod, 10, 3).should == [3, 1]
  end

  specify "should raise PrimitiveFailure when division by zero occurs" do
    run_primitive(:fixnum_divmod, 10, 0).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_divmod, -10, 0).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_divmod, -10, -0).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_divmod, 10, -0).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_divmod primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:fixnum_divmod, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_divmod, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:fixnum_divmod, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_divmod, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:fixnum_divmod, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_divmod, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_divmod, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_divmod primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:fixnum_divmod, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_divmod, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:fixnum_divmod, 1).should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_div primitive sent two fixnums" do
  specify "should return the quotient of its arguments" do
    run_primitive(:fixnum_div, 5, 5).should == 1
    run_primitive(:fixnum_div, -11, -11).should == 1
    run_primitive(:fixnum_div, -20, 5).should == -4
    run_primitive(:fixnum_div, 10, 1).should == 10
  end

  specify "should discard the remainder of a division" do
    run_primitive(:fixnum_div, 10, 3).should == 3
    run_primitive(:fixnum_div, 3, 10).should == 0
  end

  specify "should raise PrimitiveFailure when division by zero occurs" do
    run_primitive(:fixnum_div, 10, 0).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_div, -10, 0).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_div, -10, -0).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_div, 10, -0).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_div primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:fixnum_div, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_div, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:fixnum_div, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_div, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:fixnum_div, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_div, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_div, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_div primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:fixnum_div, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_div, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:fixnum_div, 1).should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_invert primitive sent a fixnum" do
  specify "should return the complement of its fixnum argument" do
    run_primitive(:fixnum_invert, 5).should == -6
    run_primitive(:fixnum_invert, 0).should == -1
    run_primitive(:fixnum_invert, -11).should == 10
  end
end

context "The fixnum_invert primitive sent a fixnum" do
  specify "should raise PrimitiveFailure with a non-fixnum" do
    run_primitive(:fixnum_invert, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_invert, 7.9).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_invert primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 0 args" do
    run_primitive(:fixnum_invert, 1, 2).should_raise(ArgumentError) # 1 args
    run_primitive(:fixnum_invert, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_invert, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end
end

context "The fixnum_modulo primitive sent two fixnums" do
  specify "should return the remainder of its arguments" do
    run_primitive(:fixnum_modulo, 5, 5).should == 0
    run_primitive(:fixnum_modulo, -11, -11).should == 0
    run_primitive(:fixnum_modulo, -20, 5).should == 0
    run_primitive(:fixnum_modulo, 10, 1).should == 0
    run_primitive(:fixnum_modulo, 10, 3).should == 1
    run_primitive(:fixnum_modulo, 3, 10).should == 3
  end

  specify "should raise PrimitiveFailure when division by zero occurs" do
    run_primitive(:fixnum_modulo, 10, 0).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_modulo, -10, 0).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_modulo, -10, -0).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_modulo, 10, -0).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_modulo primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:fixnum_modulo, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_modulo, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:fixnum_modulo, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_modulo, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:fixnum_modulo, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_modulo, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_modulo, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_modulo primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:fixnum_modulo, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_modulo, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:fixnum_modulo, 1).should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_mul primitive sent two fixnums" do
  specify "should return the product of its arguments" do
    run_primitive(:fixnum_mul, 5, 5).should == 25
    run_primitive(:fixnum_mul, 0, 0).should == 0
    run_primitive(:fixnum_mul, -11, -11).should == 121
    run_primitive(:fixnum_mul, 5, 20).should == 100
    run_primitive(:fixnum_mul, 10, 1).should == 10
  end
end

context "The fixnum_mul primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:fixnum_mul, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_mul, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:fixnum_mul, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_mul, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:fixnum_mul, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_mul, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_mul, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_mul primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:fixnum_mul, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_mul, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:fixnum_mul, 1).should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_neg primitive sent a fixnum" do
  specify "should return the negative of its fixnum argument" do
    run_primitive(:fixnum_neg, 5).should == -5
    run_primitive(:fixnum_neg, 0).should == 0
    run_primitive(:fixnum_neg, -11).should == 11
  end
end

context "The fixnum_neg primitive sent a fixnum" do
  specify "should raise PrimitiveFailure with a non-fixnum" do
    run_primitive(:fixnum_neg, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_neg, 7.9).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_neg primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 0 args" do
    run_primitive(:fixnum_neg, 1, 2).should_raise(ArgumentError) # 1 args
    run_primitive(:fixnum_neg, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_neg, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end
end

context "The fixnum_or primitive sent two fixnums" do
  specify "should return the bitwise or of its' arguments" do
    run_primitive(:fixnum_or, 500, 500).should == 500
    run_primitive(:fixnum_or, 0, 0).should == 0
    run_primitive(:fixnum_or, -11, -11).should == -11
    run_primitive(:fixnum_or, 5, 1).should == 5
    run_primitive(:fixnum_or, 2, 1).should == 3
    run_primitive(:fixnum_or, 15, 1).should == 15
    run_primitive(:fixnum_or, 15, 7).should == 15
    run_primitive(:fixnum_or, -1, 1).should == -1
    run_primitive(:fixnum_or, -2, 1).should == -1
    run_primitive(:fixnum_or, -8, 7).should == -1
    run_primitive(:fixnum_or, -16, 16).should == -16
  end
end

context "The fixnum_or primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:fixnum_or, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_or, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:fixnum_or, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_or, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:fixnum_or, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_or, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_or, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_or primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:fixnum_or, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_or, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:fixnum_or, 1).should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_size primitive sent a fixnums" do
  specify "should return the implementation byte length of fixnum" do
    run_primitive(:fixnum_size, 5).should == 4
    run_primitive(:fixnum_size, 0).should == 4
    run_primitive(:fixnum_size, -11).should == 4
  end
end

context "The fixnum_size primitive sent a fixnum" do
  specify "should raise PrimitiveFailure with a non-fixnum" do
    run_primitive(:fixnum_size, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_size, 7.9).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_size primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 0 args" do
    run_primitive(:fixnum_size, 1, 2).should_raise(ArgumentError) # 1 args
    run_primitive(:fixnum_size, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_size, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end
end

context "The fixnum_to_s primitive sent self and a base (as a fixnum)" do
  specify "should return the representation of the number in decimal" do
    run_primitive(:fixnum_to_s, 5, 10).should == '5'
    run_primitive(:fixnum_to_s, 31, 10).should == '31'
  end

  specify "should return the representation of the number in hex" do
    run_primitive(:fixnum_to_s, 5, 16).should == '5'
    run_primitive(:fixnum_to_s, 31, 16).should == '1f'
  end

  specify "should return the representation of the number in octal" do
    run_primitive(:fixnum_to_s, 5, 8).should == '5'
    run_primitive(:fixnum_to_s, 31, 8).should == '37'
  end

  specify "should return the representation of the number in binary" do
    run_primitive(:fixnum_to_s, 5, 2).should == '101'
    run_primitive(:fixnum_to_s, 31, 2).should == '11111'
  end
end

context "The fixnum_to_s primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:fixnum_to_s, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_to_s, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:fixnum_to_s, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_to_s, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:fixnum_to_s, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_to_s, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_to_s, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_to_s primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:fixnum_to_s, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_to_s, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:fixnum_to_s, 1).should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_xor primitive sent two fixnums" do
  specify "should return the bitwise xor of its' arguments" do
    run_primitive(:fixnum_xor, 500, 500).should == 0
    run_primitive(:fixnum_xor, 0, 0).should == 0
    run_primitive(:fixnum_xor, -11, -11).should == 0
    run_primitive(:fixnum_xor, 5, 1).should == 4
    run_primitive(:fixnum_xor, 2, 1).should == 3
    run_primitive(:fixnum_xor, 15, 1).should == 14
    run_primitive(:fixnum_xor, 15, 7).should == 8
    run_primitive(:fixnum_xor, -1, 1).should == -2
    run_primitive(:fixnum_xor, -2, 1).should == -1
    run_primitive(:fixnum_xor, -8, 7).should == -1
    run_primitive(:fixnum_xor, -16, 16).should == -32
  end
end

context "The fixnum_xor primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:fixnum_xor, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_xor, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:fixnum_xor, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_xor, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:fixnum_xor, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_xor, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_xor, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_xor primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:fixnum_xor, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_xor, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:fixnum_xor, 1).should_raise(ArgumentError) # 0 args
  end
end

context "The sub primitive sent two fixnums" do
  specify "should return the difference of its arguments" do
    run_primitive(:sub, 500, 500).should == 0
    run_primitive(:sub, 0, 0).should == 0
    run_primitive(:sub, -11, -11).should == 0
    run_primitive(:sub, 5, 20).should == -15
    run_primitive(:sub, 10, 1).should == 9
  end
end

context "The sub primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:sub, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:sub, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:sub, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:sub, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:sub, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:sub, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:sub, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The sub primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:sub, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:sub, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:sub, 1).should_raise(ArgumentError) # 0 args
  end
end

