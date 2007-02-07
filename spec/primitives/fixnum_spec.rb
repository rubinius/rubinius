require File.dirname(__FILE__) + '/../primitives_spec_helper'

class Fixnum
  include PrimitiveSpecHelper
  alias :old_div :/

  def /(o)
    primitive(:fixnum_div, self, o)
  end
end

# This illustrates a different approach to writing specs
# by changing the definition of an operator as shown above.
context "Primitive fixnum_div with two Fixnum arguments" do
  specify "should return self divided by other" do
    (5/5).should == 1
    (-11/-11).should == 1
    (-20/5).should == -4
    (10/1).should == 10
  end
  
  specify "should discard the remainder of dividing self by other" do
    (10/3).should == 3
    (3/10).should == 0
  end
  
  specify "should raise PrimitiveFailure when division by zero occurs" do
    (10/0).should_raise(PrimitiveFailure)
    (-10/0).should_raise(PrimitiveFailure)
    (-10/-0).should_raise(PrimitiveFailure)
    (10/-0).should_raise(PrimitiveFailure)
  end
end

context "Primitive fixnum_div with non-Fixnum arguments" do
  specify "should raise PrimitiveFailure with Fixnum, non-Fixnum" do
    (3/"foo").should_raise(PrimitiveFailure)
    (3/7.9).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_div primitive sent two fixnums" do
  include PrimitiveSpecHelper
  
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
