require File.expand_path('../../../spec_helper', __FILE__)

describe :complex_Complex_complex_complex, :shared => true do
  it "returns a new Complex number based on the two given numbers" do
    Complex(Complex(3, 4), Complex(5, 6)).should == Complex(3 - 6, 4 + 5)
    Complex(Complex(1.5, 2), Complex(-5, 6.3)).should == Complex(1.5 - 6.3, 2 - 5)
  end
end

describe :complex_Complex_complex, :shared => true do
  it "returns the passed Complex number" do
    Complex(Complex(1, 2)).should == Complex(1, 2)
    Complex(Complex(-3.4, bignum_value)).should == Complex(-3.4, bignum_value)
  end
end

describe :complex_Complex_integer_integer, :shared => true do
  it "returns a new Complex number" do
    Complex(1, 2).should be_an_instance_of(Complex)
    Complex(1, 2).real.should == 1
    Complex(1, 2).imag.should == 2

    Complex(-3, -5).should be_an_instance_of(Complex)
    Complex(-3, -5).real.should == -3
    Complex(-3, -5).imag.should == -5

    Complex(3.5, -4.5).should be_an_instance_of(Complex)
    Complex(3.5, -4.5).real.should == 3.5
    Complex(3.5, -4.5).imag.should == -4.5

    Complex(bignum_value, 30).should be_an_instance_of(Complex)
    Complex(bignum_value, 30).real.should == bignum_value
    Complex(bignum_value, 30).imag.should == 30
  end
end

describe :complex_Complex_integer, :shared => true do
  it "returns a new Complex number with 0 as the imaginary component" do
    # Guard against the Mathn library
    conflicts_with :Prime do
      Complex(1).should be_an_instance_of(Complex)
      Complex(1).imag.should == 0
      Complex(1).real.should == 1

      Complex(-3).should be_an_instance_of(Complex)
      Complex(-3).imag.should == 0
      Complex(-3).real.should == -3

      Complex(-4.5).should be_an_instance_of(Complex)
      Complex(-4.5).imag.should == 0
      Complex(-4.5).real.should == -4.5

      Complex(bignum_value).should be_an_instance_of(Complex)
      Complex(bignum_value).imag.should == 0
      Complex(bignum_value).real.should == bignum_value
    end
  end

  ruby_version_is ""..."1.9" do
    it "returns the passed Integer when Complex::Unify is defined" do
      # Guard against the Mathn library
      conflicts_with :Prime do
        begin
          Complex::Unify = true

          Complex(1).should eql(1)
          Complex(-3).should eql(-3)
          Complex(-4.5).should eql(-4.5)
          Complex(bignum_value).should eql(bignum_value)
        ensure
          Complex.send :remove_const, :Unify
        end
      end
    end
  end
end
