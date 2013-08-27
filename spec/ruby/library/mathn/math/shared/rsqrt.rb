require 'mathn'
require File.expand_path('../../fixtures/classes', __FILE__)

describe :mathn_math_rsqrt, :shared => true do
  it "returns the square root for Rational numbers" do
    @object.send(:rsqrt, Rational(9, 25)).should == Rational(3, 5)
    @object.send(:rsqrt, 16/64).should == Rational(1, 2)
  end

  it "returns the square root for positive numbers" do
    @object.send(:rsqrt, 1).should == 1
    @object.send(:rsqrt, 4.0).should == 2.0
    @object.send(:rsqrt, 12.34).should == Math.sqrt!(12.34)
  end

  ruby_version_is ""..."1.9" do
    it "raises an Errno::EDOM if the argument is a negative number" do
      lambda { @object.send(:rsqrt, -1) }.should raise_error(Errno::EDOM)
      lambda { @object.send(:rsqrt, -4.0) }.should raise_error(Errno::EDOM)
      lambda { @object.send(:rsqrt, -16/64) }.should raise_error(Errno::EDOM)
    end
  end

  ruby_version_is "1.9" do
    it "raises Math::DomainError if the argument is a negative number" do
      lambda { @object.send(:rsqrt, -1) }.should raise_error(Math::DomainError)
      lambda { @object.send(:rsqrt, -4.0) }.should raise_error(Math::DomainError)
      lambda { @object.send(:rsqrt, -16/64) }.should raise_error(Math::DomainError)
    end
  end
end
