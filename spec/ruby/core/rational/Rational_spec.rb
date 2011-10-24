ruby_version_is "1.9" do
  require File.expand_path('../../../shared/rational/Rational', __FILE__)

  describe "Rational()" do
    describe "passed two arguments" do
      it_behaves_like(:rational_rational_two, :Rational)
    end

    describe "passed Integer" do
      it_behaves_like(:rational_rational_int, :Rational)
    end

    describe "passed two integers" do
      it_behaves_like(:rational_rational_int_int, :Rational)
    end

    describe "when passed a String" do
      it "converts the String to a Rational using the same method as String#to_r" do
        r = Rational(13, 25)
        s_r = ".52".to_r
        r_s = Rational(".52")

        r_s.should == r
        r_s.should == s_r
      end

      it "scales the Rational value of the first argument by the Rational value of the second" do
        Rational(".52", ".6").should == Rational(13, 15)
        Rational(".52", "1.6").should == Rational(13, 40)
      end

      it "does not use the same method as Float#to_r" do
        r = Rational(3, 5)
        f_r = 0.6.to_r
        r_s = Rational("0.6")

        r_s.should == r
        r_s.should_not == f_r
      end
    end

    it "raises a TypeError if the arguments are not Integers" do
      lambda {
        Rational(nil)
      }.should raise_error(TypeError)

      lambda {
        Rational(nil, 1)
      }.should raise_error(TypeError)

      lambda {
        Rational(:foo, 1)
      }.should raise_error(TypeError)

      lambda {
        Rational(:foo)
      }.should raise_error(TypeError)
    end
  end
end
