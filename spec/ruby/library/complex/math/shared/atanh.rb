require File.dirname(__FILE__) + '/../fixtures/classes'

describe :complex_math_atanh, :shared => true do
  it "returns the inverse hyperbolic tangent of the argument" do
    @object.send(@method, 0.0).should == 0.0
    @object.send(@method, -0.0).should == -0.0
    @object.send(@method, 0.5).should be_close(0.549306144334055, TOLERANCE)
    @object.send(@method, -0.2).should be_close(-0.202732554054082, TOLERANCE)
  end

  ruby_version_is ""..."1.9" do
    platform_is :darwin, :freebsd, :java do
      it "returns Infinity for 1.0" do
        @object.send(@method, 1.0).infinite?.should == 1
      end

      it "returns -Infinity for -1.0" do
        @object.send(@method, -1.0).infinite?.should == -1
      end
    end

    platform_is :windows, :openbsd do
      # jruby is cross-platform and behaves as :darwin above
      not_compliant_on :jruby do
        it "raises an Errno::EDOM if x == 1.0" do
          lambda { @object.send(@method, 1.0) }.should raise_error(Errno::EDOM)
        end

        it "raises an Errno::EDOM if x == -1.0" do
          lambda { @object.send(@method, -1.0) }.should raise_error(Errno::EDOM)
        end
      end
    end

    platform_is :linux do
      it "raises an Errno::ERANGE if x == 1.0" do
        lambda { @object.send(@method, 1.0) }.should raise_error(Errno::ERANGE)
      end

      it "raises an Errno::ERANGE if x == -1.0" do
        lambda { @object.send(@method, -1.0) }.should raise_error(Errno::ERANGE)
      end
    end
  end

  ruby_version_is "1.9" do
    platform_is_not :linux do
      it "raises an Errno::EDOM if x == 1.0" do
        lambda { @object.send(@method, 1.0) }.should raise_error(Errno::EDOM)
      end

      it "raises an Errno::EDOM if x == -1.0" do
        lambda { @object.send(@method, -1.0) }.should raise_error(Errno::EDOM)
      end
    end

    platform_is :linux do
      it "raises an Errno::ERANGE if x == 1.0" do
        lambda { @object.send(@method, 1.0) }.should raise_error(Errno::ERANGE)
      end

      it "raises an Errno::ERANGE if x == -1.0" do
        lambda { @object.send(@method, -1.0) }.should raise_error(Errno::ERANGE)
      end
    end
  end
end

describe :complex_math_atanh_complex, :shared => true do
  it "returns the inverse hyperbolic tangent as a Complex number for arguments greater than 1.0" do
    value = Complex(18.36840028483855, 1.5707963267948966)
    @object.send(@method, 1.0 + Float::EPSILON).should be_close(value, TOLERANCE)

    value = Complex(0.100335347731076, 1.5707963267949)
    @object.send(@method, 10).should be_close(value, TOLERANCE)
  end

  it "returns the inverse hyperbolic tangent as a Complex number for arguments greater than 1.0" do
    value = Complex(-18.36840028483855, 1.5707963267948966)
    @object.send(@method, -1.0 - Float::EPSILON).should be_close(value, TOLERANCE)

    value = Complex(0.100335347731076, 1.5707963267949)
    @object.send(@method, 10).should be_close(value, TOLERANCE)
  end

  it "returns the inverse hyperbolic tangent for Complex numbers" do
    value = Complex(0.117500907311434, 1.40992104959658)
    @object.send(@method, Complex(3, 4)).should be_close(value, TOLERANCE)
  end
end

describe :complex_math_atanh_bang, :shared => true do
  it "raises an Errno::EDOM for arguments greater than 1.0" do
    lambda { @object.send(:atanh!, 1.0 + Float::EPSILON)  }.should raise_error(Errno::EDOM)
  end

  it "raises an Errno::EDOM for arguments less than -1.0" do
    lambda { @object.send(:atanh!, -1.0 - Float::EPSILON) }.should raise_error(Errno::EDOM)
  end

  it "raises a TypeError when passed a Complex number" do
    lambda { @object.send(:atanh!, Complex(4, 5)) }.should raise_error(TypeError)
  end
end
