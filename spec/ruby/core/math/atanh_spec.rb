require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Math.atanh" do
  it "returns a float" do
    Math.atanh(0.5).should be_an_instance_of(Float)
  end

  it "returns the inverse hyperbolic tangent of the argument" do
    Math.atanh(0.0).should == 0.0
    Math.atanh(-0.0).should == -0.0
    Math.atanh(0.5).should be_close(0.549306144334055, TOLERANCE)
    Math.atanh(-0.2).should be_close(-0.202732554054082, TOLERANCE)
  end

  platform_is :darwin, :freebsd, :java do
    ruby_version_is ""..."1.9" do
      it "returns Infinity for 1.0" do
        Math.atanh(1.0).infinite?.should == 1
      end

      it "returns -Infinity for -1.0" do
        Math.atanh(-1.0).infinite?.should == -1
      end
    end

    ruby_version_is "1.9" do
      it "raises an Errno::EDOM if x = 1.0" do
        lambda { Math.atanh(1.0) }.should raise_error(Errno::EDOM)
      end

      it "raises an Errno::EDOM if x = -1.0" do
        lambda { Math.atanh(-1.0) }.should raise_error(Errno::EDOM)
      end
    end
  end

  platform_is :windows, :linux, :openbsd do
    # jruby is cross-platform and behaves as :darwin above
    not_compliant_on :jruby do
      it "raises an Errno::EDOM if x = 1.0" do
        lambda { Math.atanh(1.0) }.should raise_error(Errno::EDOM)
      end

      it "raises an Errno::EDOM if x = -1.0" do
        lambda { Math.atanh(-1.0) }.should raise_error(Errno::EDOM)
      end
    end
  end

  conflicts_with :Complex do
    it "raises an Errno::EDOM if the passed argument is greater than 1.0" do
      lambda { Math.atanh(1.0 + TOLERANCE)  }.should raise_error(Errno::EDOM)
    end

    it "raises an Errno::EDOM if the passed argument is less than -1.0" do
      lambda { Math.atanh(-1.0 - TOLERANCE) }.should raise_error(Errno::EDOM)
    end
  end

  ruby_version_is ""..."1.9" do
    it "raises an ArgumentError if the argument cannot be coerced with Float()" do
      lambda { Math.atanh("test") }.should raise_error(ArgumentError)
    end
  end

  ruby_version_is "1.9" do
    it "raises a TypeError if the argument cannot be coerced with Float()" do
      lambda { Math.atanh("test") }.should raise_error(TypeError)
    end
  end

  it "raises a TypeError if the argument is nil" do
    lambda { Math.atanh(nil) }.should raise_error(TypeError)
  end

  it "accepts any argument that can be coerced with Float()" do
    Math.atanh(MathSpecs::Float.new(0.5)).infinite?.should be_nil
  end
end

describe "Math#atanh" do
  it "is accessible as a private instance method" do
    IncludesMath.new.send(:atanh, 0.1415).should be_close(0.14245589281616, TOLERANCE)
  end
end
