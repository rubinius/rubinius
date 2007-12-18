require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#>>" do
  before(:each) do 
    @bignum = BignumHelper.sbm(90812)
  end

  it "returns self shifted the given amount of bits to the right" do
    (@bignum >> 1).should == 536916318
    (@bignum >> 3).should == 134229079
  end

  it "performs a left-shift if given a negative value" do
    (@bignum >> -1).should == (@bignum << 1)
    (@bignum >> -3).should == (@bignum << 3)
  end
  
  it "tries to convert it's argument to an Integer using to_int" do
    (@bignum >> 1.3).should == 536916318
    
    (obj = mock('1')).should_receive(:to_int).and_return(1)
    (@bignum >> obj).should == 536916318
  end
  
  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = mock('asdf')
    lambda { @bignum >> obj }.should raise_error(TypeError)
    
    obj.should_receive(:to_int).and_return("asdf")
    lambda { @bignum >> obj }.should raise_error(TypeError)
  end

  # This test was added as the result of ruby-core:9020.
  platform :darwin do
    version '1.8'..'1.8.5' do
      it "shows the bug described in ruby-core:9020" do
        ((1 - 2**32) >> 32).should == 0
        ((1 - 2**64) >> 64).should == 0
      end
    end

    version :not, '1.8'..'1.8.5' do
      it "return the right shift alignment" do
        ((1 - 2**31) >> 31).should == -1
        ((1 - 2**32) >> 32).should == -1
        ((1 - 2**63) >> 63).should == -1 
        ((1 - 2**64) >> 64).should == -1 
      end
    end
  end

  platform :not, :darwin do
    it "return the right shift alignment" do
      ((1 - 2**31) >> 31).should == -1
      ((1 - 2**32) >> 32).should == -1
      ((1 - 2**63) >> 63).should == -1 
      ((1 - 2**64) >> 64).should == -1 
    end
  end
end
