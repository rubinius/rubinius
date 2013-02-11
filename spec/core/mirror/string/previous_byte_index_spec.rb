# -*- encoding: utf-8 -*-
require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

with_feature :encoding do
  describe "Rubinius::Mirror::String#previous_byte_index" do
    it "returns the byte index of the start of the character from a byte in that character" do
      str = encode("かた", "utf-32be")
      string_mirror(str).previous_byte_index(6).should == 4
    end

    it "returns nil if passed 0" do
      str = encode("かた", "utf-32be")
      string_mirror(str).previous_byte_index(0).should be_nil
    end

    it "raises an ArgumentError if passed a negative index" do
      lambda { string_mirror("abc").previous_byte_index(-1) }.should raise_error(ArgumentError)
    end
  end
end
