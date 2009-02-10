require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Numeric#coerce" do
  before(:each) do
    @obj = NumericSub.new
    @obj.should_receive(:to_f).any_number_of_times.and_return(10.5)
  end

  it "returns [other, self] if self and other are instances of the same class" do
    a = NumericSub.new
    b = NumericSub.new

    a.coerce(b).should == [b, a]
  end

  it "calls #to_f to convert other if self responds to #to_f" do
    other = NumericSub.new
    lambda { @obj.coerce(other) }.should raise_error(TypeError)
  end

  it "returns [other.to_f, self.to_f] if self and other are instances of different classes" do
    result = @obj.coerce(2.5)
    result.should == [2.5, 10.5]
    result.first.should be_kind_of(Float)
    result.last.should be_kind_of(Float)

    result = @obj.coerce(3)
    result.should == [3.0, 10.5]
    result.first.should be_kind_of(Float)
    result.last.should be_kind_of(Float)

    result = @obj.coerce("4.4")
    result.should == [4.4, 10.5]
    result.first.should be_kind_of(Float)
    result.last.should be_kind_of(Float)

    result = @obj.coerce(bignum_value)
    result.should == [bignum_value.to_f, 10.5]
    result.first.should be_kind_of(Float)
    result.last.should be_kind_of(Float)
  end

  it "raises a TypeError when passed nil" do
    lambda { @obj.coerce(nil)     }.should raise_error(TypeError)
  end

  it "raises a TypeError when passed a boolean" do
    lambda { @obj.coerce(false)   }.should raise_error(TypeError)
  end

  it "raises a TypeError when passed a Symbol" do
    lambda { @obj.coerce(:symbol) }.should raise_error(TypeError)
  end

  it "raises an ArgumentError when passed a String" do
    lambda { @obj.coerce("test")  }.should raise_error(ArgumentError)
  end
end
