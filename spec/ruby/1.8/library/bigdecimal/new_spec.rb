require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal.new" do

  it "creates a new object of class BigDecimal" do
    BigDecimal.new("3.14159").class.should == BigDecimal
  end

  it "Number of significant digits >= given precision" do
    pi_string = "3.1415923"
    BigDecimal.new("3.1415923", 10).precs[1].should >= 10
  end

  it "determines precision from initial value" do
    pi_string = "3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593014782083152134043"
    BigDecimal.new(pi_string).precs[1].should >= pi_string.size
  end

end
