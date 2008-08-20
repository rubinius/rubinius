require File.dirname(__FILE__) + '/../../../spec_helper'
require 'mathn'

describe "Prime#next" do
  it "returns the element at the current position and moves forward" do
    p = Prime.new
    p.next.should == 2
    p.next.should == 3
    p.next.next.should == 6
  end
end
