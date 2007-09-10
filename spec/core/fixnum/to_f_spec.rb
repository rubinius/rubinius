require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#to_f" do
  it "returns self converted to Float" do
    0.to_f.to_s.should == '0.0'
    -500.to_f.to_s.should == '-500.0'
    9_641_278.to_f.to_s.should == '9641278.0'
  end
end
