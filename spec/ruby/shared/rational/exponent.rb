require File.expand_path('../../../spec_helper', __FILE__)
require 'rational'

describe :rational_exponent_rat, :shared => true do
  conflicts_with :Prime do
    ruby_version_is ""..."1.9" do
      it "converts self to a Float and returns it raised to the passed argument" do
        (Rational(3, 4) ** Rational(4, 3)).should be_close(0.681420222312052, TOLERANCE)
        (Rational(3, 4) ** Rational(-4, 3)).should be_close(1.46752322173095, TOLERANCE)
        (Rational(3, 4) ** Rational(4, -3)).should be_close(1.46752322173095, TOLERANCE)

        (Rational(3, 4) ** Rational(0, 3)).should eql(1.0)
        (Rational(-3, 4) ** Rational(0, 3)).should eql(1.0)
        (Rational(3, -4) ** Rational(0, 3)).should eql(1.0)
        (Rational(3, 4) ** Rational(0, -3)).should eql(1.0)

        (Rational(bignum_value, 4) ** Rational(0, 3)).should eql(1.0)
        (Rational(3, -bignum_value) ** Rational(0, 3)).should eql(1.0)
        (Rational(3, 4) ** Rational(0, bignum_value)).should eql(1.0)
        (Rational(3, 4) ** Rational(0, -bignum_value)).should eql(1.0)
      end
    end

    ruby_version_is "1.9" do
      it "returns self raised to the argument as a Rational if possible" do

        (Rational(3, 4) ** Rational(0, 3)).should == Rational(1, 1)
        (Rational(-3, 4) ** Rational(0, 3)).should == Rational(1, 1)
        (Rational(3, -4) ** Rational(0, 3)).should == Rational(1, 1)
        (Rational(3, 4) ** Rational(0, -3)).should == Rational(1, 1)

        (Rational(bignum_value, 4) ** Rational(0, 3)).should == Rational(1, 1)
        (Rational(3, -bignum_value) ** Rational(0, 3)).should == Rational(1, 1)
        (Rational(3, 4) ** Rational(0, bignum_value)).should == Rational(1, 1)
        (Rational(3, 4) ** Rational(0, -bignum_value)).should == Rational(1, 1)
      end

      it "returns self raised to the argument as a Float if necessary" do
        (Rational(3, 4) ** Rational(4, 3)).should be_close(0.681420222312052, TOLERANCE)
        (Rational(3, 4) ** Rational(-4, 3)).should be_close(1.46752322173095, TOLERANCE)
        (Rational(3, 4) ** Rational(4, -3)).should be_close(1.46752322173095, TOLERANCE)
      end

      it "returns a complex number when self is negative and the passed argument is not 0" do
        (Rational(-3, 4) ** Rational(-4, 3)).should == Complex(
          -0.7337616108654732, 1.2709123906625817)
      end
    end

    ruby_version_is ""..."1.9" do
      it "returns NaN when self is negative and the passed argument is not 0" do
        (Rational(-3, 4) ** Rational(-4, 3)).nan?.should be_true
      end
    end
  end
end

describe :rational_exponent_int, :shared => true do
  it "returns the Rational value of self raised to the passed argument" do
    (Rational(3, 4) ** 4).should == Rational(81, 256)
    (Rational(3, 4) ** -4).should == Rational(256, 81)
    (Rational(-3, 4) ** -4).should == Rational(256, 81)
    (Rational(3, -4) ** -4).should == Rational(256, 81)

    (Rational(bignum_value, 4) ** 4).should == Rational(28269553036454149273332760011886696253239742350009903329945699220681916416, 1)
    (Rational(3, bignum_value) ** -4).should == Rational(7237005577332262213973186563042994240829374041602535252466099000494570602496, 81)
    (Rational(-bignum_value, 4) ** -4).should == Rational(1, 28269553036454149273332760011886696253239742350009903329945699220681916416)
    (Rational(3, -bignum_value) ** -4).should == Rational(7237005577332262213973186563042994240829374041602535252466099000494570602496, 81)
  end

  conflicts_with :Prime do
    it "returns Rational(1, 1) when the passed argument is 0" do
      (Rational(3, 4) ** 0).should eql(Rational(1, 1))
      (Rational(-3, 4) ** 0).should eql(Rational(1, 1))
      (Rational(3, -4) ** 0).should eql(Rational(1, 1))

      (Rational(bignum_value, 4) ** 0).should eql(Rational(1, 1))
      (Rational(3, -bignum_value) ** 0).should eql(Rational(1, 1))
    end
  end
end

describe :rational_exponent_float, :shared => true do
  it "returns self converted to Float and raised to the passed argument" do
    (Rational(3, 1) ** 3.0).should eql(27.0)
    (Rational(3, 1) ** 1.5).should be_close(5.19615242270663, TOLERANCE)
    (Rational(3, 1) ** -1.5).should be_close(0.192450089729875, TOLERANCE)
  end

  ruby_version_is ""..."1.9" do
    it "returns NaN if self is negative and the passed argument is not 0" do
      (Rational(-3, 2) ** 1.5).nan?.should be_true
      (Rational(3, -2) ** 1.5).nan?.should be_true
      (Rational(3, -2) ** -1.5).nan?.should be_true
    end

    it "returns 1.0 when the passed argument is 0.0" do
      (Rational(3, 4) ** 0.0).should eql(1.0)
      (Rational(-3, 4) ** 0.0).should eql(1.0)
      (Rational(-3, 4) ** 0.0).should eql(1.0)
    end
  end

  ruby_version_is "1.9" do
    it "returns a complex number if self is negative and the passed argument is not 0" do
      (Rational(-3, 2) ** 1.5).should be_close(Complex(
        -3.374618290464398e-16, -1.8371173070873836), TOLERANCE)
      (Rational(3, -2) ** 1.5).should be_close(Complex(
        -3.374618290464398e-16, -1.8371173070873836), TOLERANCE)
      (Rational(3, -2) ** -1.5).should be_close(Complex(
        -9.998869008783402e-17, 0.5443310539518174), TOLERANCE)
    end

    it "returns Complex(1.0) when the passed argument is 0.0" do
      (Rational(3, 4) ** 0.0).should == Complex(1.0)
      (Rational(-3, 4) ** 0.0).should == Complex(1.0)
      (Rational(-3, 4) ** 0.0).should == Complex(1.0)
    end
  end
end

describe :rational_exponent, :shared => true do
  it "calls #coerce on the passed argument with self" do
    rational = Rational(3, 4)
    obj = mock("Object")
    obj.should_receive(:coerce).with(rational).and_return([1, 2])

    rational ** obj
  end

  it "calls #** on the coerced Rational with the coerced Object" do
    rational = Rational(3, 4)

    coerced_rational = mock("Coerced Rational")
    coerced_rational.should_receive(:**).and_return(:result)

    coerced_obj = mock("Coerced Object")

    obj = mock("Object")
    obj.should_receive(:coerce).and_return([coerced_rational, coerced_obj])

    (rational ** obj).should == :result
  end
end
