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
