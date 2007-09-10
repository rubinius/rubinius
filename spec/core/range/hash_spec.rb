require File.dirname(__FILE__) + '/../../spec_helper'

describe "Range#hash" do
  it "is provided" do
    (0..1).respond_to?(:hash).should == true
    ('A'..'Z').respond_to?(:hash).should == true
    (0xfffd..0xffff).respond_to?(:hash).should == true
    (0.5..2.4).respond_to?(:hash).should == true
  end  
end
