require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#size" do
  platform_is :size => 32 do
    it "returns the number of bytes in the machine representation of self" do
      -1.size.should == 4
      0.size.should == 4
      4091.size.should == 4
    end
  end

  platform_is :size => 64 do
    it "returns the number of bytes in the machine representation of self" do
      -1.size.should == 8
      0.size.should == 8
      4091.size.should == 8
    end
  end
end
