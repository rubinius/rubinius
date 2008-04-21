require File.dirname(__FILE__) + '/../../spec_helper'

# Equivalent to Numeric#/, but overridden in subclasses.
describe "Numeric#quo" do
  it "returns the floating-point result of self divided by other" do
    # the to_f is required because RSpec (I'm assuming) requires 'rational'
    2.quo(2.5).to_s.should == '0.8'
    5.quo(2).to_f.to_s.should == '2.5'
  end

  it "does not raise an exception when other is zero" do
    # 1.quo(0) should also not raise (i.e works in irb and from a file),
    # but fails here.
    1.quo(0.0).to_s.should == 'Infinity'
    1.quo(-0.0).to_s.should == '-Infinity'
    bignum_value.quo(0.0).to_s.should == 'Infinity'
  end

  it "returns the floating-point result of dividing two Fixnums" do
    13.quo(4).should be_close(3.25, TOLERANCE)
    4.quo(13).should be_close( 0.307692307692308, TOLERANCE)
  end

  it "returns the floating-point result of dividing a Fixnum by a Float" do
    13.quo(4.0).should be_close(3.25, TOLERANCE)
    4.quo(13.0).should be_close(0.307692307692308, TOLERANCE)
  end

  it "returns the floating-point result of dividing two Floats" do
    13.0.quo(4.0).should be_close(3.25, TOLERANCE)
    4.0.quo(13.0).should be_close(0.307692307692308, TOLERANCE)
  end

  it "returns the floating_point result of dividing a Bignum by a Fixnum" do
    bignum_value.quo(100).should == 92233720368547760.0
  end

  it "raises an ArgumentError when not passed one argument" do
    lambda { 13.quo       }.should raise_error(ArgumentError)
    lambda { 13.quo(1, 2) }.should raise_error(ArgumentError)
  end

  conflicts_with :Rational do
    it "raises a TypeError when not passed a Numeric type" do
      lambda { 13.quo(nil)    }.should raise_error(TypeError)
      lambda { 13.quo('test') }.should raise_error(TypeError)
      lambda { 13.quo(true)   }.should raise_error(TypeError)
    end
  end
end
