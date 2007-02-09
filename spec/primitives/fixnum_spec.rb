require File.dirname(__FILE__) + '/../primitives_spec_helper'

context "The add primitive sent two fixnums" do
  specify "should return the sum of its arguments" do
    500.prim.add(500).should == 1000
    0.prim.add( 0).should == 0
    -11.prim.add( -11).should == -22
  end
end

context "The add primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    3.prim.add( "foo").should_raise(PrimitiveFailure)
    3.prim.add( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    "foo".prim.add( 80).should_raise(PrimitiveFailure)
    100.0.prim.add( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    "foo".prim.add( "bar").should_raise(PrimitiveFailure)
    "foo".prim.add( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.add( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The add primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.add( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.add( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.add.should_raise(ArgumentError) # 0 args
  end
end

context "The compare primitive sent two fixnums" do
  specify "should return 1 if x > y" do
    30.prim.compare( 20).should == 1
    10.prim.compare( -5).should == 1
    -10.prim.compare( -500).should == 1
  end

  specify "should return 0 if x == y" do
    500.prim.compare( 500).should == 0
    0.prim.compare( 0).should == 0
    -11.prim.compare( -11).should == 0
  end

  specify "should return -1 if x < y" do
    3.prim.compare( 500).should == -1
    -8.prim.compare( 3).should == -1
    -1000.prim.compare( -20).should == -1
  end

end

context "The compare primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    3.prim.compare( "foo").should_raise(PrimitiveFailure)
    3.prim.compare( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    "foo".prim.compare( 80).should_raise(PrimitiveFailure)
    100.0.prim.compare( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    "foo".prim.compare( "bar").should_raise(PrimitiveFailure)
    "foo".prim.compare( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.compare( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The compare primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.compare( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.compare( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.compare.should_raise(ArgumentError) # 0 args
  end
end

context "The equal primitive sent two fixnums" do
  specify "should return true if x == y" do
    500.prim.equal( 500).should == true
    0.prim.equal( 0).should == true
    -11.prim.equal( -11).should == true
  end

  specify "should return false if x != y" do
    3.prim.equal( 500).should == false
    -8.prim.equal( 3).should == false
    -1000.prim.equal( -20).should == false
  end
end

context "The equal primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    3.prim.equal( "foo").should_raise(PrimitiveFailure)
    3.prim.equal( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    "foo".prim.equal( 80).should_raise(PrimitiveFailure)
    100.0.prim.equal( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    "foo".prim.equal( "bar").should_raise(PrimitiveFailure)
    "foo".prim.equal( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.equal( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The equal primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.equal( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.equal( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.equal.should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_and primitive sent two fixnums" do
  specify "should return the bitwise and of its' arguments" do
    500.prim.fixnum_and( 500).should == 500
    0.prim.fixnum_and( 0).should == 0
    -11.prim.fixnum_and( -11).should == -11
    5.prim.fixnum_and( 1).should == 1
    2.prim.fixnum_and( 1).should == 0
    15.prim.fixnum_and( 1).should == 1
    15.prim.fixnum_and( 7).should == 7
    -1.prim.fixnum_and( 1).should == 1
    -2.prim.fixnum_and( 1).should == 0
    -8.prim.fixnum_and( 7).should == 0
    -16.prim.fixnum_and( 16).should == 16
  end
end

context "The fixnum_and primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    3.prim.fixnum_and( "foo").should_raise(PrimitiveFailure)
    3.prim.fixnum_and( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    "foo".prim.fixnum_and( 80).should_raise(PrimitiveFailure)
    100.0.prim.fixnum_and( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    "foo".prim.fixnum_and( "bar").should_raise(PrimitiveFailure)
    "foo".prim.fixnum_and( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.fixnum_and( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_and primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.fixnum_and( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.fixnum_and( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.fixnum_and.should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_divmod primitive sent two fixnums" do
  specify "should return the quotient and remainder of its arguments" do
    5.prim.fixnum_divmod( 5).should == [1, 0]
    -11.prim.fixnum_divmod( -11).should == [1, 0]
    -20.prim.fixnum_divmod( 5).should == [-4, 0]
    10.prim.fixnum_divmod( 1).should == [10, 0]
    3.prim.fixnum_divmod( 320).should == [0, 3]
    10.prim.fixnum_divmod( 50).should == [0, 10]
    10.prim.fixnum_divmod( 3).should == [3, 1]
  end

  specify "should raise PrimitiveFailure when division by zero occurs" do
    10.prim.fixnum_divmod( 0).should_raise(PrimitiveFailure)
    -10.prim.fixnum_divmod( 0).should_raise(PrimitiveFailure)
    -10.prim.fixnum_divmod( -0).should_raise(PrimitiveFailure)
    10.prim.fixnum_divmod( -0).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_divmod primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    3.prim.fixnum_divmod( "foo").should_raise(PrimitiveFailure)
    3.prim.fixnum_divmod( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    "foo".prim.fixnum_divmod( 80).should_raise(PrimitiveFailure)
    100.0.prim.fixnum_divmod( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    "foo".prim.fixnum_divmod( "bar").should_raise(PrimitiveFailure)
    "foo".prim.fixnum_divmod( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.fixnum_divmod( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_divmod primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.fixnum_divmod( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.fixnum_divmod( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.fixnum_divmod.should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_div primitive sent two fixnums" do
  specify "should return the quotient of its arguments" do
    5.prim.fixnum_div( 5).should == 1
    -11.prim.fixnum_div( -11).should == 1
    -20.prim.fixnum_div( 5).should == -4
    10.prim.fixnum_div( 1).should == 10
  end

  specify "should discard the remainder of a division" do
    10.prim.fixnum_div( 3).should == 3
    3.prim.fixnum_div( 10).should == 0
  end

  specify "should raise PrimitiveFailure when division by zero occurs" do
    10.prim.fixnum_div( 0).should_raise(PrimitiveFailure)
    -10.prim.fixnum_div( 0).should_raise(PrimitiveFailure)
    -10.prim.fixnum_div( -0).should_raise(PrimitiveFailure)
    10.prim.fixnum_div( -0).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_div primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    3.prim.fixnum_div( "foo").should_raise(PrimitiveFailure)
    3.prim.fixnum_div( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    "foo".prim.fixnum_div( 80).should_raise(PrimitiveFailure)
    100.0.prim.fixnum_div( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    "foo".prim.fixnum_div( "bar").should_raise(PrimitiveFailure)
    "foo".prim.fixnum_div( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.fixnum_div( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_div primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.fixnum_div( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.fixnum_div( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.fixnum_div.should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_invert primitive sent a fixnum" do
  specify "should return the complement of its fixnum argument" do
    5.prim.fixnum_invert.should == -6
    0.prim.fixnum_invert.should == -1
    -11.prim.fixnum_invert.should == 10
  end
end

context "The fixnum_invert primitive sent a fixnum" do
  specify "should raise PrimitiveFailure with a non-fixnum" do
    "foo".prim.fixnum_invert.should_raise(PrimitiveFailure)
    7.9.prim.fixnum_invert.should_raise(PrimitiveFailure)
  end
end

context "The fixnum_invert primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 0 args" do
    1.prim.fixnum_invert( 2).should_raise(ArgumentError) # 1 args
    1.prim.fixnum_invert( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.fixnum_invert( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end
end

context "The fixnum_modulo primitive sent two fixnums" do
  specify "should return the remainder of its arguments" do
    5.prim.fixnum_modulo( 5).should == 0
    -11.prim.fixnum_modulo( -11).should == 0
    -20.prim.fixnum_modulo( 5).should == 0
    10.prim.fixnum_modulo( 1).should == 0
    10.prim.fixnum_modulo( 3).should == 1
    3.prim.fixnum_modulo( 10).should == 3
  end

  specify "should raise PrimitiveFailure when division by zero occurs" do
    10.prim.fixnum_modulo( 0).should_raise(PrimitiveFailure)
    -10.prim.fixnum_modulo( 0).should_raise(PrimitiveFailure)
    -10.prim.fixnum_modulo( -0).should_raise(PrimitiveFailure)
    10.prim.fixnum_modulo( -0).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_modulo primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    3.prim.fixnum_modulo( "foo").should_raise(PrimitiveFailure)
    3.prim.fixnum_modulo( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    "foo".prim.fixnum_modulo( 80).should_raise(PrimitiveFailure)
    100.0.prim.fixnum_modulo( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    "foo".prim.fixnum_modulo( "bar").should_raise(PrimitiveFailure)
    "foo".prim.fixnum_modulo( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.fixnum_modulo( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_modulo primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.fixnum_modulo( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.fixnum_modulo( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.fixnum_modulo.should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_mul primitive sent two fixnums" do
  specify "should return the product of its arguments" do
    5.prim.fixnum_mul( 5).should == 25
    0.prim.fixnum_mul( 0).should == 0
    -11.prim.fixnum_mul( -11).should == 121
    5.prim.fixnum_mul( 20).should == 100
    10.prim.fixnum_mul( 1).should == 10
  end
end

context "The fixnum_mul primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    3.prim.fixnum_mul( "foo").should_raise(PrimitiveFailure)
    3.prim.fixnum_mul( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    "foo".prim.fixnum_mul( 80).should_raise(PrimitiveFailure)
    100.0.prim.fixnum_mul( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    "foo".prim.fixnum_mul( "bar").should_raise(PrimitiveFailure)
    "foo".prim.fixnum_mul( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.fixnum_mul( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_mul primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.fixnum_mul( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.fixnum_mul( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.fixnum_mul.should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_neg primitive sent a fixnum" do
  specify "should return the negative of its fixnum argument" do
    5.prim.fixnum_neg.should == -5
    0.prim.fixnum_neg.should == 0
    -11.prim.fixnum_neg.should == 11
  end
end

context "The fixnum_neg primitive sent a fixnum" do
  specify "should raise PrimitiveFailure with a non-fixnum" do
    "foo".prim.fixnum_neg.should_raise(PrimitiveFailure)
    7.9.prim.fixnum_neg.should_raise(PrimitiveFailure)
  end
end

context "The fixnum_neg primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 0 args" do
    1.prim.fixnum_neg( 2).should_raise(ArgumentError) # 1 args
    1.prim.fixnum_neg( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.fixnum_neg( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end
end

context "The fixnum_or primitive sent two fixnums" do
  specify "should return the bitwise or of its' arguments" do
    500.prim.fixnum_or( 500).should == 500
    0.prim.fixnum_or( 0).should == 0
    -11.prim.fixnum_or( -11).should == -11
    5.prim.fixnum_or( 1).should == 5
    2.prim.fixnum_or( 1).should == 3
    15.prim.fixnum_or( 1).should == 15
    15.prim.fixnum_or( 7).should == 15
    -1.prim.fixnum_or( 1).should == -1
    -2.prim.fixnum_or( 1).should == -1
    -8.prim.fixnum_or( 7).should == -1
    -16.prim.fixnum_or( 16).should == -16
  end
end

context "The fixnum_or primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    3.prim.fixnum_or( "foo").should_raise(PrimitiveFailure)
    3.prim.fixnum_or( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    "foo".prim.fixnum_or( 80).should_raise(PrimitiveFailure)
    100.0.prim.fixnum_or( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    "foo".prim.fixnum_or( "bar").should_raise(PrimitiveFailure)
    "foo".prim.fixnum_or( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.fixnum_or( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_or primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.fixnum_or( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.fixnum_or( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.fixnum_or.should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_size primitive sent a fixnums" do
  specify "should return the implementation byte length of fixnum" do
    5.prim.fixnum_size.should == 4
    0.prim.fixnum_size.should == 4
    -11.prim.fixnum_size.should == 4
  end
end

context "The fixnum_size primitive sent a non-fixnum" do
  specify "should raise PrimitiveFailure" do
    "foo".prim.fixnum_size.should_raise(PrimitiveFailure)
    7.9.prim.fixnum_size.should_raise(PrimitiveFailure)
  end
end

context "The fixnum_size primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 0 args" do
    1.prim.fixnum_size( 2).should_raise(ArgumentError) # 1 args
    1.prim.fixnum_size( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.fixnum_size( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end
end

context "The fixnum_to_s primitive sent self and a base (as a fixnum)" do
  specify "should return the representation of the number in decimal" do
    5.prim.fixnum_to_s( 10).should == '5'
    31.prim.fixnum_to_s( 10).should == '31'
  end

  specify "should return the representation of the number in hex" do
    5.prim.fixnum_to_s( 16).should == '5'
    31.prim.fixnum_to_s( 16).should == '1f'
  end

  specify "should return the representation of the number in octal" do
    5.prim.fixnum_to_s( 8).should == '5'
    31.prim.fixnum_to_s( 8).should == '37'
  end

  specify "should return the representation of the number in binary" do
    5.prim.fixnum_to_s( 2).should == '101'
    31.prim.fixnum_to_s( 2).should == '11111'
  end
end

context "The fixnum_to_s primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    3.prim.fixnum_to_s( "foo").should_raise(PrimitiveFailure)
    3.prim.fixnum_to_s( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    "foo".prim.fixnum_to_s( 80).should_raise(PrimitiveFailure)
    100.0.prim.fixnum_to_s( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    "foo".prim.fixnum_to_s( "bar").should_raise(PrimitiveFailure)
    "foo".prim.fixnum_to_s( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.fixnum_to_s( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_to_s primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.fixnum_to_s( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.fixnum_to_s( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.fixnum_to_s.should_raise(ArgumentError) # 0 args
  end
end

context "The fixnum_xor primitive sent two fixnums" do
  specify "should return the bitwise xor of its' arguments" do
    500.prim.fixnum_xor( 500).should == 0
    0.prim.fixnum_xor( 0).should == 0
    -11.prim.fixnum_xor( -11).should == 0
    5.prim.fixnum_xor( 1).should == 4
    2.prim.fixnum_xor( 1).should == 3
    15.prim.fixnum_xor( 1).should == 14
    15.prim.fixnum_xor( 7).should == 8
    -1.prim.fixnum_xor( 1).should == -2
    -2.prim.fixnum_xor( 1).should == -1
    -8.prim.fixnum_xor( 7).should == -1
    -16.prim.fixnum_xor( 16).should == -32
  end
end

context "The fixnum_xor primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    3.prim.fixnum_xor( "foo").should_raise(PrimitiveFailure)
    3.prim.fixnum_xor( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    "foo".prim.fixnum_xor( 80).should_raise(PrimitiveFailure)
    100.0.prim.fixnum_xor( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    "foo".prim.fixnum_xor( "bar").should_raise(PrimitiveFailure)
    "foo".prim.fixnum_xor( 38.8).should_raise(PrimitiveFailure)
    8.8.prim.fixnum_xor( 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_xor primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.fixnum_xor( 2, 3).should_raise(ArgumentError) # 2 args
    1.prim.fixnum_xor( 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    1.prim.fixnum_xor.should_raise(ArgumentError) # 0 args
  end
end

context "The sub primitive sent two fixnums" do
  specify "should return the difference of its arguments" do
    500.prim.sub( 500).should == 0
    0.prim.sub( 0).should == 0
    -11.prim.sub( -11).should == 0
    5.prim.sub( 20).should == -15
    10.prim.sub( 1).should == 9
  end
end

context "The sub primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    3.prim.sub( "foo").should_raise(PrimitiveFailure)
    3.prim.sub( 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    "foo".prim.sub( 80).should_raise(PrimitiveFailure)
    100.0.prim.sub( 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
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

context "The fixnum_to_f primitive sent a fixnum" do
  specify "should return floating point representation equivalent" do
    8.prim.fixnum_to_f.should == 8.0
    540.prim.fixnum_to_f.should == 540.0
  end
end

context "The fixnum_to_f primitive sent a non-fixnum" do
  specify "should raise PrimitiveFailure" do
    "foo".prim.fixnum_to_f.should_raise(PrimitiveFailure)
    7.9.prim.fixnum_to_f.should_raise(PrimitiveFailure)
  end
end

context "The fixnum_to_f primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    1.prim.fixnum_to_f( 2).should_raise(ArgumentError) # 1 args
    1.prim.fixnum_to_f( 2, 3).should_raise(ArgumentError) # 2 args
  end
end

# TODO: fixnum_shift (would like to simplify primitive actually but evan says NO
