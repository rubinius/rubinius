require File.dirname(__FILE__) + '/../../spec_helper'

describe "Regexp#casefold?" do
  it "returns the value of the case-insensitive flag" do
    /abc/i.casefold?.should == true
    /xyz/.casefold?.should == false
  end
end
