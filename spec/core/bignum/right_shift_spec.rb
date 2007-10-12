require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#>>" do
  it "returns self shifted right other bits" do
    a = BignumHelper.sbm(90812)
    (a >> 3.45).should == 134229079
    (a >> 2).should == 268458159
    (a >> 21).should == 512
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
