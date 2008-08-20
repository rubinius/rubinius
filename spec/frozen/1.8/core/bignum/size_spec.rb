require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum#size" do
  it "returns the number of bytes in the machine representation of self" do
    compliant_on(:ruby) do
      (256**7).size.should == 8
      (256**8).size.should == 12
      (256**9).size.should == 12
      (256**10).size.should == 12
      (256**10-1).size.should == 12
      (256**11).size.should == 12
      (256**12).size.should == 16
      (256**20-1).size.should == 20
      (256**40-1).size.should == 40
    end

    compliant_on(:rubinius, :jruby) do
      (256**7).size.should == 8
      (256**8).size.should == 9
      (256**9).size.should == 10
      (256**10).size.should == 11
      (256**10-1).size.should == 10
      (256**11).size.should == 12
      (256**12).size.should == 13
      (256**20-1).size.should == 20
      (256**40-1).size.should == 40
    end
  end
end
