require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#*" do
  it "returns self multiplied by other" do
    (4923 * 2).should == 9846
    (1342177 * 800).should == 1073741600
    (65536 * 65536).should == 4294967296
  end

  it "coerces fixnum and return self multiplied by other" do 
    (256 * 0xffffffff).should == 1099511627520
    (6712 * 0.25).to_s.should == '1678.0'
  end
end
