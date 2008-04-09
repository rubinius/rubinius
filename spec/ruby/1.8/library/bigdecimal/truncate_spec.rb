require File.dirname(__FILE__) + '/../../spec_helper'
require 'bigdecimal'

describe "BigDecimal#truncate" do

  before(:each) do
      @arr = ['3.14159', '8.7', "0.314159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593014782083152134043E1"]
      @big = BigDecimal("123456.789")
  end

  it "returns value of type Bigdecimal." do
    @arr.each do |x|
      BigDecimal(x).truncate.kind_of?(BigDecimal).should == true
    end
  end

  it "returns the nearest Integer if no precision given" do
    BigDecimal(@arr[0]).truncate == 3
    BigDecimal(@arr[1]).truncate == 8
    BigDecimal(@arr[2]).truncate == 3
  end

  it "returns value of given precision otherwise" do
    BigDecimal(@arr[0]).truncate(2).should == BigDecimal("3.14")
    BigDecimal(@arr[1]).truncate(1).should == BigDecimal("8.7")
    BigDecimal(@arr[2]).truncate(100).should == BigDecimal(\
      "3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679")
  end

  it "sets n digits left of the decimal point to 0, if given n < 0" do
    @big.truncate(-1).should == BigDecimal("123450.0")
    @big.truncate(-2).should == BigDecimal("123400.0")
    BigDecimal(@arr[2]).truncate(-1).should == 0
  end

end


