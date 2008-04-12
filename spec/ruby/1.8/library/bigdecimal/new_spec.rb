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

  it "ignores leading whitespace" do
    BigDecimal.new("  \t\n \r1234").should == BigDecimal.new("1234")
  end

  it "ignores trailing garbage" do
    BigDecimal.new("123E45ruby").should == BigDecimal.new("123E45")
    BigDecimal.new("123x45").should == BigDecimal.new("123")
    BigDecimal.new("123.4%E5").should == BigDecimal.new("123.4")
  end

  it "treats invalid strings as 0.0" do
    BigDecimal.new("ruby").should == BigDecimal.new("0.0")
  end

  it "allows omitting the integer part" do
    BigDecimal.new(".123").should == BigDecimal.new("0.123")
    BigDecimal.new("-.123").should == BigDecimal.new("-0.123")
  end

  it "allows for underscores in all parts" do
    reference = BigDecimal.new("12345.67E89")

    BigDecimal.new("12_345.67E89").should == reference
    BigDecimal.new("1_2_3_4_5_._6____7_E89").should == reference
    BigDecimal.new("12345_.67E_8__9_").should == reference
  end

  it "accepts NaN and [+-]Infinity" do
    BigDecimal.new("NaN").nan?.should == true

    pos_inf = BigDecimal.new("Infinity")
    pos_inf.finite?.should == false
    pos_inf.should > 0
    pos_inf.should == BigDecimal.new("+Infinity")

    neg_inf = BigDecimal.new("-Infinity")
    neg_inf.finite?.should == false
    neg_inf.should < 0
  end

  it "allows for [eEdD] as exponent separator" do
    reference = BigDecimal.new("12345.67E89")

    BigDecimal.new("12345.67e89").should == reference
    BigDecimal.new("12345.67E89").should == reference
    BigDecimal.new("12345.67d89").should == reference
    BigDecimal.new("12345.67D89").should == reference
  end

  it "allows for varying signs" do
    reference = BigDecimal.new("123.456E1")

    BigDecimal.new("+123.456E1").should == reference
    BigDecimal.new("-123.456E1").should == -reference
    BigDecimal.new("123.456E+1").should == reference
    BigDecimal.new("12345.6E-1").should == reference
    BigDecimal.new("+123.456E+1").should == reference
    BigDecimal.new("+12345.6E-1").should == reference
    BigDecimal.new("-123.456E+1").should == -reference
    BigDecimal.new("-12345.6E-1").should == -reference
  end

end
