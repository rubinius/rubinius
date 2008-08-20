require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Numeric#singleton_method_added" do
  it "raises a TypeError when trying to #dup a Numeric" do
    lambda do
      a = NumericSub.new
      a.dup
    end.should raise_error(TypeError)
    
    lambda do
      a = 1
      a.dup
    end.should raise_error(TypeError)

    lambda do
      a = 1.5
      a.dup
    end.should raise_error(TypeError)

    lambda do
      a = bignum_value
      a.dup
    end.should raise_error(TypeError)
  end
end