require File.dirname(__FILE__) + '/../spec_helper'

context "Bignum instance method" do
  specify "coerce should return [Bignum, Bignum] if other is a Fixnum" do
    example do
      @a = 0xffffffff.coerce(1)
      [@a, @a.collect { |i| i.class }]
    end.should == [[1, 4294967295], [Bignum, Bignum]]
  end
  
  specify "coerce should return [Float, Float] if other is not a Bignum or Fixnum" do
    example do
      @a = 0xffffffff.coerce("2")
      @b = 0xffffffff.coerce(1.5)
      [@a, @a.collect { |i| i.class }, @b, @b.collect { |i| i.class }]
    end.should == [[2.0, 4294967295.0], [Float, Float], [1.5, 4294967295.0], [Float, Float]]
  end
end
