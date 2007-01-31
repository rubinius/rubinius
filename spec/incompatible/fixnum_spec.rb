require File.dirname(__FILE__) + '/../spec_helper'

context "Fixnum instance method" do
  specify "coerce should return [Bignum, Bignum] if other is a Bignum" do
    example do
      @a = 1.coerce(0xffffffff)
      [@a, @a.collect { |i| i.class }]
    end.should == [[4294967295, 1], [Bignum, Bignum]]
  end
end
