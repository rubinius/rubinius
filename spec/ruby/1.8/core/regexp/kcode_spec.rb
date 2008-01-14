require File.dirname(__FILE__) + '/../../spec_helper'

describe "Regexp#kcode" do
  it "returns the character set code" do
    /f.(o)/.kcode.should == 'none'
    /ab+c/s.kcode.should == "sjis"
    /a(.)+s/n.kcode.should == "none"
    /xyz/e.kcode.should == "euc"
    /cars/u.kcode.should == "utf8"
  end
end
