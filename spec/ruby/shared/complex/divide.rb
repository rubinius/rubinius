require File.expand_path('../../../spec_helper', __FILE__)

describe :complex_divide_complex, :shared => true do
  it "divides according to the usual rule for complex numbers" do
    a = Complex((1 * 10) - (2 * 20), (1 * 20) + (2 * 10))
    b = Complex(1, 2)
    (a / b).should == Complex(10, 20)

    c = Complex((1.5 * 100.2) - (2.1 * -30.3), (1.5 * -30.3) + (2.1 * 100.2))
    d = Complex(1.5, 2.1)
    # remember the floating-point arithmetic
    (c / d).should be_close(Complex(100.2, -30.3), TOLERANCE)
  end
end

describe :complex_divide_fixnum, :shared => true do
  it "divides both parts of the Complex number" do
    (Complex(20, 40) / 2).should == Complex(10, 20)
    (Complex(30, 30) / 10).should == Complex(3, 3)
  end

  it "raises a ZeroDivisionError when given zero" do
    lambda { Complex(20, 40) / 0 }.should raise_error(ZeroDivisionError)
  end
end

describe :complex_divide_bignum, :shared => true do
  it "divides both parts of the Complex number" do
    (Complex(20, 40) / 2).should == Complex(10, 20)
    (Complex(15, 16) / 2.0).should be_close(Complex(7.5, 8), TOLERANCE)
  end
end

describe :complex_divide_float, :shared => true do
  it "divides both parts of the Complex number" do
    (Complex(3, 9) / 1.5).should == Complex(2, 6)
    (Complex(15, 16) / 2.0).should be_close(Complex(7.5, 8), TOLERANCE)
  end

  it "returns Complex(Infinity, Infinity) when given zero" do
    (Complex(20, 40) / 0.0).real.infinite?.should == 1
    (Complex(20, 40) / 0.0).imag.infinite?.should == 1
    (Complex(-20, 40) / 0.0).real.infinite?.should == -1
    (Complex(-20, 40) / 0.0).imag.infinite?.should == 1
  end

end

describe :complex_divide_object, :shared => true do
  it "tries to coerce self into other" do
    value = Complex(3, 9)

    obj = mock("Object")
    obj.should_receive(:coerce).with(value).and_return([4, 2])
    (value / obj).should == 2
  end
end
