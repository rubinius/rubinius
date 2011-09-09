require File.expand_path('../../../../spec_helper', __FILE__)

describe "Array#pack with format 'P'" do
  it "returns a String whose size is the number of bytes in a mochine word" do
    [nil].pack("P").size.should == 1.size
  end
end
