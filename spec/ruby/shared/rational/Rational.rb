require File.expand_path('../../../spec_helper', __FILE__)

describe :rational_rational_two, :shared => true do
  before(:each) do
    @args = {
      [1, 2]   => [1, 2],
      [2, 1]   => [2, 1],
      ["6", 5] => [6, 5],
      [-8, -6] => [4, 3],
      [99999**99, 209] => [999010484943188636608805980402802915400434536979655386654009490813859457598616206837179821412302692420911878238392114694353872205705469979997758000847544074497398071603034467574810862492956315403164145181317850364010376300213453752437982265518464568325790559557401725276413045564927258325699353130042548208064924009216093547546101231747525975999315735437260590254281341103015581933843191009768771215462495272328209801935290153265929807401919949633636861142033871459231063562556848951490009899999, 209],
      [30.21, 0.22] => [362809985980967168, 2642111781390691],
      [-28, 2.3] => [-31525197391593472, 2589569785738035]
    }
  end

  it "returns a Rational instance" do
    @args.each_pair do |arguments, expected|
      rat = Rational(arguments.first, arguments.last)
      rat.should be_an_instance_of(Rational)
    end
  end

  it "sets the numerator to the first argument" do
    @args.each_pair do |arguments, expected|
      rat = Rational(arguments.first, arguments.last)
      rat.numerator.should == expected.first
    end
  end

  it "sets the denominator to the second argument" do
    @args.each_pair do |arguments, expected|
      rat = Rational(arguments.first, arguments.last)
      rat.denominator.should == expected.last
    end
  end

  it "automatically reduces the Rational" do
    rat = Rational(2, 4)
    rat.numerator.should == 1
    rat.denominator.should == 2

    rat = Rational(3, 9)
    rat.numerator.should == 1
    rat.denominator.should == 3
  end
end

describe :rational_rational_int_int, :shared => true do
  it "returns a new Rational number" do
    rat = Rational(1, 2)
    rat.numerator.should == 1
    rat.denominator.should == 2
    rat.should be_an_instance_of(Rational)

    rat = Rational(-3, -5)
    rat.numerator.should == 3
    rat.denominator.should == 5
    rat.should be_an_instance_of(Rational)

    rat = Rational(bignum_value, 3)
    rat.numerator.should == bignum_value
    rat.denominator.should == 3
    rat.should be_an_instance_of(Rational)
  end

  it "automatically reduces the Rational" do
    rat = Rational(2, 4)
    rat.numerator.should == 1
    rat.denominator.should == 2

    rat = Rational(3, 9)
    rat.numerator.should == 1
    rat.denominator.should == 3
  end
end


describe :rational_rational_int, :shared => true do
  # Guard against the Mathn library
  conflicts_with :Prime do
    ruby_version_is ""..."1.9" do
      it "returns a new Rational number with 1 as the denominator" do
        Rational(1).should eql(Rational.new!(1, 1))
        Rational(-3).should eql(Rational.new!(-3, 1))
        Rational(bignum_value).should eql(Rational.new!(bignum_value, 1))
      end
    end

    ruby_version_is "1.9" do
      it "returns a new Rational number with 1 as the denominator" do
        Rational(1).should eql(Rational(1, 1))
        Rational(-3).should eql(Rational(-3, 1))
        Rational(bignum_value).should eql(Rational(bignum_value, 1))
      end
    end
  end
end

ruby_version_is ""..."1.9" do
  describe :rational_rational_int_rat_unify, :shared => true do
    after :each do
      Rational.send :remove_const, :Unify
    end

    it "returns the passed Integer when Rational::Unify is defined" do
      Rational::Unify = true

      Rational(1).should eql(1)
      Rational(-3).should eql(-3)
      Rational(bignum_value).should eql(bignum_value)
    end
  end
end
