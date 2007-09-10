require File.dirname(__FILE__) + '/../../spec_helper'

describe "Integer#downto" do
  it "passes block decreasing values from self down to and including other Integer" do
    a = []
    3.downto(-1) { |i| a << i }
    -1.downto(3) { |i| a << i }
    a.should == [3, 2, 1, 0, -1]
  end
end
