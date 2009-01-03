require File.dirname(__FILE__) + '/../../spec_helper'
require 'rational'

describe "Rational#** when passed [Rational]" do
  conflicts_with :Prime do
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

    it "returns NaN when self is negative and the passed argument is not 0" do
      (Rational(-3, 4) ** Rational(-4, 3)).nan?.should be_true
    end
  end
end

describe "Rational#** when passed [Integer]" do
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

describe "Rational#** when passed [Float]" do
  it "returns self converted to Float and raised to the passed argument" do
    (Rational(3, 1) ** 3.0).should eql(27.0)
    (Rational(3, 1) ** 1.5).should be_close(5.19615242270663, TOLERANCE)
    (Rational(3, 1) ** -1.5).should be_close(0.192450089729875, TOLERANCE)
  end
  
  it "returns 1.0 when the passed argument is 0" do
    (Rational(3, 4) ** 0.0).should eql(1.0)
    (Rational(-3, 4) ** 0.0).should eql(1.0)
    (Rational(-3, 4) ** 0.0).should eql(1.0)
  end
  
  it "returns NaN if self is negative and the passed argument is not 0" do
    (Rational(-3, 2) ** 1.5).nan?.should be_true
    (Rational(3, -2) ** 1.5).nan?.should be_true
    (Rational(3, -2) ** -1.5).nan?.should be_true
  end
end

describe "Rational#**" do
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
