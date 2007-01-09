require File.dirname(__FILE__) + '/../spec_helper'

context "Rubinius Bignum" do
  specify "should have max value 2 ** 29" do
    example do
      @max = 2 ** 29
      p [@max.class, (@max - 1).class]
    end.should == '[Bignum, Fixnum]'
  end
end
