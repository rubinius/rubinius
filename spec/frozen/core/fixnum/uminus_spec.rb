require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#-@" do
  it "returns self as a negative value" do
    2.send(:-@).should == -2
    -2.should == -2
    -268435455.should == -268435455
    (--5).should == 5
    -8.send(:-@).should == 8
  end
end
