require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Numeric#divmod" do
  before(:each) do
    @obj = NumericSub.new
  end

  ruby_version_is ""..."1.9" do
    it "returns [quotient, modulus], with quotient being obtained as in Numeric#div and modulus being obtained by calling self#% with other" do
      @obj.should_receive(:/).with(10).and_return(13 - TOLERANCE)
      @obj.should_receive(:%).with(10).and_return(3)
      
      @obj.divmod(10).should == [12, 3]
    end
  end

  ruby_version_is "1.9" do
    it "returns [quotient, modulus], with quotient being obtained as in Numeric#div then #floor and modulus being obtained by calling self#- with quotient * other" do
      @obj.should_receive(:/).twice.with(10).and_return(13 - TOLERANCE, 13 - TOLERANCE)
      @obj.should_receive(:-).with(120).and_return(3)

      @obj.divmod(10).should == [12, 3]
    end
  end
end
