require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#*" do
  it "returns self multiplied by other" do
    a = BignumHelper.sbm(772)
    (a * 98.6).should_be_close(1.0587101996559999e+11, TOLERANCE)
    (a * 10).to_s.should == '10737425960'
    (a * (a - 40)).to_s.should == '1152923119515115376'
  end
end
