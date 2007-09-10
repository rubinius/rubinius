require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#quo" do
  it "returns the floating-point result of self divided by other" do
    # the to_f is required because RSpec (I'm assuming) requires 'rational'
    2.quo(2.5).to_s.should == '0.8'
    5.quo(2).to_f.to_s.should == '2.5'
    45.quo(0xffffffff).to_f.to_s.should == '1.04773789668636e-08'
  end
  
  it "does NOT raise an exception when other is zero" do
    # 1.quo(0) should also not raise (i.e works in irb and from a file),
    # but fails here.
    1.quo(0.0).to_s.should == 'Infinity'
    1.quo(-0.0).to_s.should == '-Infinity'
  end
end
