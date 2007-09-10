require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#<<" do
  it "returns self shifted left other bits" do
    (7 << 2).should == 28
    (9 << 4).should == 144
  end
  
  it "coerces result on overflow and return self shifted left other bits" do
    (9 << 4.2).should == 144
    (6 << 0xff).should == 347376267711948586270712955026063723559809953996921692118372752023739388919808
  end
end
