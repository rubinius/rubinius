require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#abs" do
  it "returns self's absolute value" do
    { 0 => [0, -0, +0], 2 => [2, -2, +2], 100 => [100, -100, +100] }.each do |key, values|
      values.each do |value|
        value.abs.should == key
      end
    end
  end
end
