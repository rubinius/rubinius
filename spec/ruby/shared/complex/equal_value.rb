require File.expand_path('../../../spec_helper', __FILE__)

describe :complex_equal_value_complex, :shared => true do
  it "returns true when self and other have numerical equality" do
    Complex(1, 2).should == Complex(1, 2)
    Complex(3, 9).should == Complex(3, 9)
    Complex(-3, -9).should == Complex(-3, -9)

    Complex(1, 2).should_not == Complex(3, 4)
    Complex(3, 9).should_not == Complex(9, 3)

    Complex(1.0, 2.0).should == Complex(1, 2)
    Complex(3.0, 9.0).should_not == Complex(9.0, 3.0)

    Complex(1.5, 2.5).should == Complex(1.5, 2.5)
    Complex(1.5, 2.5).should == Complex(1.5, 2.5)
    Complex(-1.5, 2.5).should == Complex(-1.5, 2.5)

    Complex(1.5, 2.5).should_not == Complex(2.5, 1.5)
    Complex(3.75, 2.5).should_not == Complex(1.5, 2.5)

    Complex(bignum_value, 2.5).should == Complex(bignum_value, 2.5)
    Complex(3.75, bignum_value).should_not == Complex(1.5, bignum_value)

    Complex(nan_value).should_not == Complex(nan_value)
  end
end

describe :complex_equal_value_numeric, :shared => true do
  it "returns true when self's imaginary part is 0 and the real part and other have numerical equality" do
    Complex(3, 0).should == 3
    Complex(-3, 0).should == -3

    Complex(3.5, 0).should == 3.5
    Complex(-3.5, 0).should == -3.5

    Complex(bignum_value, 0).should == bignum_value
    Complex(-bignum_value, 0).should == -bignum_value

    Complex(3.0, 0).should == 3
    Complex(-3.0, 0).should == -3

    Complex(3, 0).should_not == 4
    Complex(-3, 0).should_not == -4

    Complex(3.5, 0).should_not == -4.5
    Complex(-3.5, 0).should_not == 2.5

    Complex(bignum_value, 0).should_not == bignum_value(10)
    Complex(-bignum_value, 0).should_not == -bignum_value(20)
  end
end

describe :complex_equal_value_object, :shared => true do
  # Fixnum#==, Float#== and Bignum#== only return booleans - Bug?
  it "calls other#== with self" do
    value = Complex(3, 0)

    obj = mock("Object")
    obj.should_receive(:==).with(value).and_return(:expected)

    (value == obj).should == :expected
  end
end
