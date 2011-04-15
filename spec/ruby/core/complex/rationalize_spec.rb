ruby_version_is "1.9" do

  describe "Complex#rationalize" do

    it "raises RangeError if self has non-zero imaginary part" do
      lambda { Complex(1,5).rationalize }.should raise_error(RangeError)
    end

    it "returns a Rational if self has zero imaginary part" do
      Complex(1,0).rationalize.should == Rational(1,1)
      Complex(2<<63+5).rationalize.should == Rational(2<<63+5,1)
    end

  end
end
