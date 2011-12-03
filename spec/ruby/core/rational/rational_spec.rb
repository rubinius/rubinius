ruby_version_is "1.9" do
  describe "Rational#rational?" do
    it "returns true" do
      Rational(2, 3).rational?.should == true
    end
  end
end
