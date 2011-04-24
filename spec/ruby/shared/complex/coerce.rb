require File.expand_path('../../../spec_helper', __FILE__)

describe :complex_coerce, :shared => true do
  before(:each) do
    @one = Complex(1)
  end

  it "returns an array containing other and self as Complex when other is an Integer" do
    result = @one.coerce(2)
    result.should == [2, 1]
    result.first.should be_kind_of(Complex)
    result.last.should be_kind_of(Complex)
  end

  it "returns an array containing other and self as Complex when other is a Float" do
    result = @one.coerce(20.5)
    result.should == [20.5, 1]
    result.first.should be_kind_of(Complex)
    result.last.should be_kind_of(Complex)
  end

  it "returns an array containing other and self as Complex when other is a Bignum" do
    result = @one.coerce(4294967296)
    result.should == [4294967296, 1]
    result.first.should be_kind_of(Complex)
    result.last.should be_kind_of(Complex)
  end

  it "returns an array containing other and self as Complex when other is a Rational" do
    result = @one.coerce(Rational(5,6))
    result.should == [Rational(5,6), 1]
    result.first.should be_kind_of(Complex)
    result.last.should be_kind_of(Complex)
  end

  it "raises a TypeError when other is a String" do
    lambda { @one.coerce("20") }.should raise_error(TypeError)
    lambda { @one.coerce(nil)   }.should raise_error(TypeError)
    lambda { @one.coerce(false) }.should raise_error(TypeError)
  end
end
