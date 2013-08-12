require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "Rational#marshal_dump" do
    ruby_version_is "1.9"..."2.0" do
      it "is a public method" do
        Rational.should have_public_instance_method(:marshal_dump, false)
      end
    end

    ruby_version_is "2.0" do
      it "is a private method" do
        Rational.should have_private_instance_method(:marshal_dump, false)
      end
    end

    it "dumps numerator and denominator" do
      Rational(1, 2).send(:marshal_dump).should == [1, 2]
    end
  end
end
