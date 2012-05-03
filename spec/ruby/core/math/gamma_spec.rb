require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "Math.gamma" do
    it "returns +infinity given 0" do
      Math.gamma(0).should == Float::INFINITY
    end

    it "returns -infinity given -0.0" do
      Math.gamma(-0.0).should == -Float::INFINITY
    end

    it "returns Math.sqrt(Math::PI) given 0.5" do
      Math.gamma(0.5).should be_close(Math.sqrt(Math::PI), TOLERANCE)
    end

    # stop at n == 23 because 23! cannot be exactly represented by IEEE 754 double
    f = 1
    2.upto(23) do |n|
      it "returns exactly #{n-1}! given #{n}" do
        f *= n - 1
        Math.gamma(n).should == f
      end
    end

    f = 1124000727777607680000  # 22!
    24.upto(30) do |n|
      it "returns approximately #{n-1}! given #{n}" do
        f *= n - 1
        # compare only the first 12 places, tolerate the rest
        Math.gamma(n).should be_close(f, f.to_s[12..-1].to_i)
      end
    end

    it "returns good numerical approximation for gamma(3.2)" do
      Math.gamma(3.2).should be_close(2.423965, TOLERANCE)
    end

    it "returns good numerical approximation for gamma(-2.15)" do
      Math.gamma(-2.15).should be_close(-2.999619, TOLERANCE)
    end

    it "returns good numerical approximation for gamma(0.00001)" do
      Math.gamma(0.00001).should be_close(99999.422794, TOLERANCE)
    end

    it "returns good numerical approximation for gamma(-0.00001)" do
      Math.gamma(-0.00001).should be_close(-100000.577225, TOLERANCE)
    end

    it "raises Math::DomainError given -1" do
      lambda { Math.gamma(-1) }.should raise_error(Math::DomainError)
    end

    # See http://redmine.ruby-lang.org/issues/show/2189
    it "returns +infinity given +infinity" do
      Math.gamma(Float::INFINITY).should == Float::INFINITY
    end

    it "raises Math::DomainError given negative infinity" do
      lambda { Math.gamma(-Float::INFINITY) }.should raise_error(Math::DomainError)
    end

    it "returns NaN given NaN" do
      Math.gamma(nan_value).nan?.should be_true
    end
  end
end
