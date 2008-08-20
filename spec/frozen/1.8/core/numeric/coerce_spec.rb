require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Numeric#coerce" do
  before(:each) do
    @obj = NumericSub.new
  end
  
  it "returns [other, self] if self and other are instances of the same class" do
    other = NumericSub.new
    @obj.coerce(other).should == [other, @obj]
  end
  
  it "returns an Array containing other and self converted to Fixnums using #to_f if they are not instances of the same class" do
    @obj.should_receive(:to_f).at_most(4).times.and_return(10.5)

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

    not_compliant_on :rubinius do
      result = @obj.coerce(bignum_value)
      result.should == [bignum_value.to_f, 10.5]
      result.first.should be_kind_of(Float)
      result.last.should be_kind_of(Float)
    end
  end

  it "raises a TypeError when other can't be coerced" do
    @obj.should_receive(:to_f).exactly(2).times.and_return(10.5)
    lambda { @obj.coerce(nil)   }.should raise_error(TypeError)
    lambda { @obj.coerce(false) }.should raise_error(TypeError)    
  end
  
  it "raises an ArgumentError when other can't be converted to Float" do
    @obj.should_receive(:to_f).and_return(10.5)
    lambda { @obj.coerce("test") }.should raise_error(ArgumentError)
  end
end