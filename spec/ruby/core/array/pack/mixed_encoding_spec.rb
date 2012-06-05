# -*- encoding: ascii-8bit -*-
require File.expand_path('../../../../spec_helper', __FILE__)

describe "Array#pack with mixed format" do
  ruby_version_is "1.9" do
    it "returns an ASCII-8BIT string" do
      [17,"This is a string!"].pack("NA*").encoding.should == Encoding::ASCII_8BIT
    end
  end

  ruby_version_is "1.9" do
    it "returns an ASCII-8BIT string" do
      [170,0,"This is a string!"].pack("NCA*").encoding.should == Encoding::ASCII_8BIT
    end
  end
end