require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#divmod" do
  it "returns an [quotient, modulus] from dividing self by other" do
    3.14.divmod(2).inspect.should == "[1, 1.14]"
    2.8284.divmod(3.1415).inspect.should == "[0, 2.8284]"
    -1.0.divmod(0xffffffff).inspect.should == "[-1, 4294967294.0]"
  end

  version '1.8.4' do
    it "returns [NaN, NaN] if other is zero" do
      1.0.divmod(0).inspect.should == '[NaN, NaN]'
      1.0.divmod(0.0).inspect.should == '[NaN, NaN]'
    end
  end
  
  version '1.8.5'..'1.8.6' do
    it "raises FloatDomainError if other is zero" do
      lambda { 1.0.divmod(0)   }.should raise_error(FloatDomainError)
      lambda { 1.0.divmod(0.0) }.should raise_error(FloatDomainError)
    end
  end
end
