require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#coerce when given a Fixnum or Bignum" do
  it "returns an Array containing the given argument and self" do
    a = BignumHelper.sbm
    a.coerce(2).should == [2, a]
    
    b = BignumHelper.sbm(701)
    a.coerce(b).should == [b, a]
  end
end

describe "Bignum#coerce" do
  it "raises a TypeError when given a non Fixnum/Bignum" do
    a = BignumHelper.sbm

    should_raise(TypeError) { a.coerce(nil) }
    should_raise(TypeError) { a.coerce(Object.new) }
    should_raise(TypeError) { a.coerce(1..4) }
    should_raise(TypeError) { a.coerce(:test) }

    compliant :mri do
      should_raise(TypeError) { a.coerce(12.3) }
      should_raise(TypeError) { a.coerce("123") }
    end
  end
end