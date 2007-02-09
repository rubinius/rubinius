require File.dirname(__FILE__) + '/../primitives_spec_helper'

BIGNUM1 = (1 << 50)
BIGNUM2 = (1 << 44)

context "The bignum_add primitive sent two bignums" do
  specify "should return the sum of its arguments" do
    BIGNUM1.prim.bignum_add(BIGNUM2).should == BIGNUM1 + BIGNUM2
    BIGNUM2.prim.bignum_add(BIGNUM1).should == BIGNUM1 + BIGNUM2
  end
end

context "The bignum_add primitive sent non-bignums" do
  specify "should raise PrimitiveFailure with bignum, non-bignum" do
    BIGNUM1.prim.bignum_add(3).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, bignum" do
    "foo".prim.bignum_add(BIGNUM1).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, non-bignum" do
    "foo".prim.bignum_add( "bar").should_raise(PrimitiveFailure)
  end
end

context "The bignum_add primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    BIGNUM1.prim.bignum_add(BIGNUM1, BIGNUM2).should_raise(ArgumentError) # 2 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    BIGNUM1.prim.bignum_add.should_raise(ArgumentError) # 0 args
  end
end

context "The bignum_compare primitive sent two bignums" do
  specify "should return 1 if x > y" do
    BIGNUM1.prim.bignum_compare(BIGNUM2).should == 1
  end

  specify "should return 0 if x == y" do
    BIGNUM1.prim.bignum_compare(BIGNUM1).should == 0
  end

  specify "should return -1 if x < y" do
    BIGNUM2.prim.bignum_compare(BIGNUM1).should == -1
  end

end

context "The bignum_compare primitive sent non-bignums" do
  specify "should raise PrimitiveFailure with bignum, non-bignum" do
    BIGNUM1.prim.bignum_compare('foo').should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, bignum" do
    "foo".prim.bignum_compare(BIGNUM1).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, non-bignum" do
    8.8.prim.bignum_compare(38.8).should_raise(PrimitiveFailure)
  end
end

context "The bignum_compare primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    BIGNUM1.prim.bignum_compare(BIGNUM1, BIGNUM2).should_raise(ArgumentError) # 2 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    BIGNUM1.prim.bignum_compare.should_raise(ArgumentError) # 0 args
  end
end

context "The bignum_equal primitive sent two bignums" do
  specify "should return true if x == y" do
    BIGNUM1.prim.bignum_equal(BIGNUM1).should == true
    BIGNUM2.prim.bignum_equal(BIGNUM2).should == true
  end

  specify "should return false if x != y" do
    BIGNUM1.prim.bignum_equal(BIGNUM2).should == false
  end
end

context "The bignum_equal primitive sent non-bignums" do
  specify "should raise PrimitiveFailure with bignum, non-bignum" do
    BIGNUM1.prim.bignum_equal("foo").should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, bignum" do
    "foo".prim.bignum_equal(BIGNUM1).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, non-bignum" do
    "foo".prim.bignum_equal( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The bignum_equal primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    BIGNUM1.prim.bignum_equal(BIGNUM2, BIGNUM2).should_raise(ArgumentError) # 2 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    BIGNUM1.prim.bignum_equal.should_raise(ArgumentError) # 0 args
  end
end

context "The bignum_and primitive sent two bignums" do
  specify "should return the bitwise and of its' arguments" do
    BIGNUM1.prim.bignum_and(BIGNUM2).should == BIGNUM1 & BIGNUM2
    BIGNUM1.prim.bignum_and(BIGNUM1).should == BIGNUM1 & BIGNUM1
  end
end

context "The bignum_and primitive sent non-bignums" do
  specify "should raise PrimitiveFailure with bignum, non-bignum" do
    BIGNUM1.prim.bignum_and( "foo").should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, bignum" do
    100.0.prim.bignum_and(BIGNUM1).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, non-bignum" do
    "foo".prim.bignum_and( "bar").should_raise(PrimitiveFailure)
  end
end

context "The bignum_and primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    BIGNUM1.prim.bignum_and(BIGNUM1, BIGNUM2).should_raise(ArgumentError) # 2 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    BIGNUM1.prim.bignum_and.should_raise(ArgumentError) # 0 args
  end
end

context "The bignum_divmod primitive sent two bignums" do
  specify "should return the quotient and remainder of its arguments" do
    BIGNUM1.prim.bignum_divmod(BIGNUM2).should == BIGNUM1.divmod(BIGNUM2)
    BIGNUM2.prim.bignum_divmod(BIGNUM1).should == BIGNUM2.divmod(BIGNUM1)
  end
end

context "The bignum_divmod primitive sent non-bignums" do
  specify "should raise PrimitiveFailure with bignum, non-bignum" do
    BIGNUM1.prim.bignum_divmod("foo").should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, bignum" do
    "foo".prim.bignum_divmod(BIGNUM1).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, non-bignum" do
    "foo".prim.bignum_divmod( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The bignum_divmod primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    BIGNUM1.prim.bignum_divmod(BIGNUM1, BIGNUM2).should_raise(ArgumentError) # 2 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    BIGNUM1.prim.bignum_divmod.should_raise(ArgumentError) # 0 args
  end
end

=begin

context "The bignum_div primitive sent two bignums" do
  specify "should return the quotient of its arguments" do
    5.prim.bignum_div( 5).should == 1
    -11.prim.bignum_div( -11).should == 1
    -20.prim.bignum_div( 5).should == -4
    10.prim.bignum_div( 1).should == 10
  end

  specify "should discard the remainder of a division" do
    10.prim.bignum_div( 3).should == 3
    3.prim.bignum_div( 10).should == 0
  end

  specify "should raise PrimitiveFailure when division by zero occurs" do
    10.prim.bignum_div( 0).should_raise(PrimitiveFailure)
    -10.prim.bignum_div( 0).should_raise(PrimitiveFailure)
    -10.prim.bignum_div( -0).should_raise(PrimitiveFailure)
    10.prim.bignum_div( -0).should_raise(PrimitiveFailure)
  end
end

context "The bignum_div primitive sent non-bignums" do
  specify "should raise PrimitiveFailure with bignum, non-bignum" do
    3.prim.bignum_div( "foo").should_raise(PrimitiveFailure)
    3.prim.bignum_div( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, bignum" do
    "foo".prim.bignum_div( 80).should_raise(PrimitiveFailure)
    100.0.prim.bignum_div( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, non-bignum" do
    "foo".prim.bignum_div( "bar").should_raise(PrimitiveFailure)
    "foo".prim.bignum_div( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.bignum_div( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The bignum_div primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.bignum_div( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.bignum_div( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.bignum_div.should_raise(ArgumentError) # 0 args
  end
end

context "The bignum_invert primitive sent a bignum" do
  specify "should return the complement of its bignum argument" do
    5.prim.bignum_invert.should == -6
    0.prim.bignum_invert.should == -1
    -11.prim.bignum_invert.should == 10
  end
end

context "The bignum_invert primitive sent a bignum" do
  specify "should raise PrimitiveFailure with a non-bignum" do
    "foo".prim.bignum_invert.should_raise(PrimitiveFailure)
    7.9.prim.bignum_invert.should_raise(PrimitiveFailure)
  end
end

context "The bignum_invert primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 0 args" do
    1.prim.bignum_invert( 2).should_raise(ArgumentError) # 1 args
    1.prim.bignum_invert( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.bignum_invert( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end
end

context "The bignum_modulo primitive sent two bignums" do
  specify "should return the remainder of its arguments" do
    5.prim.bignum_modulo( 5).should == 0
    -11.prim.bignum_modulo( -11).should == 0
    -20.prim.bignum_modulo( 5).should == 0
    10.prim.bignum_modulo( 1).should == 0
    10.prim.bignum_modulo( 3).should == 1
    3.prim.bignum_modulo( 10).should == 3
  end

  specify "should raise PrimitiveFailure when division by zero occurs" do
    10.prim.bignum_modulo( 0).should_raise(PrimitiveFailure)
    -10.prim.bignum_modulo( 0).should_raise(PrimitiveFailure)
    -10.prim.bignum_modulo( -0).should_raise(PrimitiveFailure)
    10.prim.bignum_modulo( -0).should_raise(PrimitiveFailure)
  end
end

context "The bignum_modulo primitive sent non-bignums" do
  specify "should raise PrimitiveFailure with bignum, non-bignum" do
    3.prim.bignum_modulo( "foo").should_raise(PrimitiveFailure)
    3.prim.bignum_modulo( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, bignum" do
    "foo".prim.bignum_modulo( 80).should_raise(PrimitiveFailure)
    100.0.prim.bignum_modulo( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, non-bignum" do
    "foo".prim.bignum_modulo( "bar").should_raise(PrimitiveFailure)
    "foo".prim.bignum_modulo( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.bignum_modulo( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The bignum_modulo primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.bignum_modulo( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.bignum_modulo( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.bignum_modulo.should_raise(ArgumentError) # 0 args
  end
end

context "The bignum_mul primitive sent two bignums" do
  specify "should return the product of its arguments" do
    5.prim.bignum_mul( 5).should == 25
    0.prim.bignum_mul( 0).should == 0
    -11.prim.bignum_mul( -11).should == 121
    5.prim.bignum_mul( 20).should == 100
    10.prim.bignum_mul( 1).should == 10
  end
end

context "The bignum_mul primitive sent non-bignums" do
  specify "should raise PrimitiveFailure with bignum, non-bignum" do
    3.prim.bignum_mul( "foo").should_raise(PrimitiveFailure)
    3.prim.bignum_mul( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, bignum" do
    "foo".prim.bignum_mul( 80).should_raise(PrimitiveFailure)
    100.0.prim.bignum_mul( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, non-bignum" do
    "foo".prim.bignum_mul( "bar").should_raise(PrimitiveFailure)
    "foo".prim.bignum_mul( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.bignum_mul( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The bignum_mul primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.bignum_mul( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.bignum_mul( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.bignum_mul.should_raise(ArgumentError) # 0 args
  end
end

context "The bignum_neg primitive sent a bignum" do
  specify "should return the negative of its bignum argument" do
    5.prim.bignum_neg.should == -5
    0.prim.bignum_neg.should == 0
    -11.prim.bignum_neg.should == 11
  end
end

context "The bignum_neg primitive sent a bignum" do
  specify "should raise PrimitiveFailure with a non-bignum" do
    "foo".prim.bignum_neg.should_raise(PrimitiveFailure)
    7.9.prim.bignum_neg.should_raise(PrimitiveFailure)
  end
end

context "The bignum_neg primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 0 args" do
    1.prim.bignum_neg( 2).should_raise(ArgumentError) # 1 args
    1.prim.bignum_neg( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.bignum_neg( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end
end

context "The bignum_or primitive sent two bignums" do
  specify "should return the bitwise or of its' arguments" do
    500.prim.bignum_or( 500).should == 500
    0.prim.bignum_or( 0).should == 0
    -11.prim.bignum_or( -11).should == -11
    5.prim.bignum_or( 1).should == 5
    2.prim.bignum_or( 1).should == 3
    15.prim.bignum_or( 1).should == 15
    15.prim.bignum_or( 7).should == 15
    -1.prim.bignum_or( 1).should == -1
    -2.prim.bignum_or( 1).should == -1
    -8.prim.bignum_or( 7).should == -1
    -16.prim.bignum_or( 16).should == -16
  end
end

context "The bignum_or primitive sent non-bignums" do
  specify "should raise PrimitiveFailure with bignum, non-bignum" do
    3.prim.bignum_or( "foo").should_raise(PrimitiveFailure)
    3.prim.bignum_or( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, bignum" do
    "foo".prim.bignum_or( 80).should_raise(PrimitiveFailure)
    100.0.prim.bignum_or( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, non-bignum" do
    "foo".prim.bignum_or( "bar").should_raise(PrimitiveFailure)
    "foo".prim.bignum_or( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.bignum_or( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The bignum_or primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.bignum_or( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.bignum_or( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.bignum_or.should_raise(ArgumentError) # 0 args
  end
end

context "The bignum_size primitive sent a bignums" do
  specify "should return the implementation byte length of bignum" do
    5.prim.bignum_size.should == 4
    0.prim.bignum_size.should == 4
    -11.prim.bignum_size.should == 4
  end
end

context "The bignum_size primitive sent a non-bignum" do
  specify "should raise PrimitiveFailure" do
    "foo".prim.bignum_size.should_raise(PrimitiveFailure)
    7.9.prim.bignum_size.should_raise(PrimitiveFailure)
  end
end

context "The bignum_size primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 0 args" do
    1.prim.bignum_size( 2).should_raise(ArgumentError) # 1 args
    1.prim.bignum_size( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.bignum_size( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end
end

context "The bignum_to_s primitive sent self and a base (as a bignum)" do
  specify "should return the representation of the number in decimal" do
    5.prim.bignum_to_s( 10).should == '5'
    31.prim.bignum_to_s( 10).should == '31'
  end

  specify "should return the representation of the number in hex" do
    5.prim.bignum_to_s( 16).should == '5'
    31.prim.bignum_to_s( 16).should == '1f'
  end

  specify "should return the representation of the number in octal" do
    5.prim.bignum_to_s( 8).should == '5'
    31.prim.bignum_to_s( 8).should == '37'
  end

  specify "should return the representation of the number in binary" do
    5.prim.bignum_to_s( 2).should == '101'
    31.prim.bignum_to_s( 2).should == '11111'
  end
end

context "The bignum_to_s primitive sent non-bignums" do
  specify "should raise PrimitiveFailure with bignum, non-bignum" do
    3.prim.bignum_to_s( "foo").should_raise(PrimitiveFailure)
    3.prim.bignum_to_s( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, bignum" do
    "foo".prim.bignum_to_s( 80).should_raise(PrimitiveFailure)
    100.0.prim.bignum_to_s( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, non-bignum" do
    "foo".prim.bignum_to_s( "bar").should_raise(PrimitiveFailure)
    "foo".prim.bignum_to_s( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.bignum_to_s( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The bignum_to_s primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.bignum_to_s( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.bignum_to_s( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.bignum_to_s.should_raise(ArgumentError) # 0 args
  end
end

context "The bignum_xor primitive sent two bignums" do
  specify "should return the bitwise xor of its' arguments" do
    500.prim.bignum_xor( 500).should == 0
    0.prim.bignum_xor( 0).should == 0
    -11.prim.bignum_xor( -11).should == 0
    5.prim.bignum_xor( 1).should == 4
    2.prim.bignum_xor( 1).should == 3
    15.prim.bignum_xor( 1).should == 14
    15.prim.bignum_xor( 7).should == 8
    -1.prim.bignum_xor( 1).should == -2
    -2.prim.bignum_xor( 1).should == -1
    -8.prim.bignum_xor( 7).should == -1
    -16.prim.bignum_xor( 16).should == -32
  end
end

context "The bignum_xor primitive sent non-bignums" do
  specify "should raise PrimitiveFailure with bignum, non-bignum" do
    3.prim.bignum_xor( "foo").should_raise(PrimitiveFailure)
    3.prim.bignum_xor( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, bignum" do
    "foo".prim.bignum_xor( 80).should_raise(PrimitiveFailure)
    100.0.prim.bignum_xor( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, non-bignum" do
    "foo".prim.bignum_xor( "bar").should_raise(PrimitiveFailure)
    "foo".prim.bignum_xor( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.bignum_xor( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The bignum_xor primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.bignum_xor( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.bignum_xor( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.bignum_xor.should_raise(ArgumentError) # 0 args
  end
end

context "The sub primitive sent two bignums" do
  specify "should return the difference of its arguments" do
    500.prim.sub( 500).should == 0
    0.prim.sub( 0).should == 0
    -11.prim.sub( -11).should == 0
    5.prim.sub( 20).should == -15
    10.prim.sub( 1).should == 9
  end
end

context "The sub primitive sent non-bignums" do
  specify "should raise PrimitiveFailure with bignum, non-bignum" do
    3.prim.sub( "foo").should_raise(PrimitiveFailure)
    3.prim.sub( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, bignum" do
    "foo".prim.sub( 80).should_raise(PrimitiveFailure)
    100.0.prim.sub( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-bignum, non-bignum" do
    "foo".prim.sub( "bar").should_raise(PrimitiveFailure)
    "foo".prim.sub( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.sub( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The sub primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.sub( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.sub( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.sub.should_raise(ArgumentError) # 0 args
  end
end

context "The bignum_to_f primitive sent a bignum" do
  specify "should return floating point representation equivalent" do
    8.prim.bignum_to_f.should == 8.0
    540.prim.bignum_to_f.should == 540.0
  end
end

context "The bignum_to_f primitive sent a non-bignum" do
  specify "should raise PrimitiveFailure" do
    "foo".prim.bignum_to_f.should_raise(PrimitiveFailure)
    7.9.prim.bignum_to_f.should_raise(PrimitiveFailure)
  end
end

context "The bignum_to_f primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.bignum_to_f( 2).should_raise(ArgumentError) # 1 args
    1.prim.bignum_to_f( 2, 3).should_raise(ArgumentError) # 2 args
  end
end
=end

# TODO: bignum_shift (would like to simplify primitive actually but evan says NO
# TODO: fill in dynamically generated should == expected results to constants
