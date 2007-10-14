require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#size" do
 it "returns the number of bytes in the machine representation of self" do
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
end
