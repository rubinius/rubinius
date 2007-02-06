require File.dirname(__FILE__) + '/../spec_helper'

context "ByteArray instance method" do
  specify "[] should return the byte at index" do
    example do
      @s= "A92f"
      @b = @s.data
      @a = []
      0.upto(@s.size-1) { |i| @a << @b[i].chr }
      @a
    end.should == ["A", "9", "2", "f"]
  end
  
  specify "[]= should set the byte at index" do
    example do
      @s = "01234"
      @b = @s.data
      0.upto(@s.size-1) { |i| @b[i] = @b[i] + ?A }
      @s
    end.should == "qrstu"
  end
end