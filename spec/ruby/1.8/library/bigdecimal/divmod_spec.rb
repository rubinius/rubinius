require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/modulo'
require 'bigdecimal'

describe "BigDecimal#mod_part_of_divmod" do
  # BigDecimal#divmod[1] behaves exactly like #modulo
  begin
    class BigDecimal
      def mod_part_of_dimvod(arg)
        divmod(arg)[1]
      end
    end
    it_behaves_like(:bigdecimal_modulo, :mod_part_of_dimvod, :exclude_float_zero)
  ensure
    class BigDecimal
      undef mod_part_of_dimvod
    end
  end
end

describe "BigDecimal#divmod" do

  before(:each) do
    @a = BigDecimal("42.00000000000000000001")
    @one = BigDecimal("1")
  end


  it "divides value, returns an array" do
    res = @a.divmod(5)
    res.kind_of?(Array).should == true
  end

  it "array contains quotient and modulus as BigDecimal" do
    res = @a.divmod(5)
    res.each do |x|
      x.kind_of?(BigDecimal).should == true
    end
    res[0].should == BigDecimal('0.8E1')
    res[1].should == BigDecimal('2.00000000000000000001')

    BigDecimal('1').divmod(BigDecimal('2')).should == [0, 1]
    BigDecimal('2').divmod(BigDecimal('1')).should == [2, 0]

    BigDecimal('1').divmod(BigDecimal('-2')).should == [-1, -1]
    BigDecimal('2').divmod(BigDecimal('-1')).should == [-2, 0]

    BigDecimal('-1').divmod(BigDecimal('2')).should == [-1, 1]
    BigDecimal('-2').divmod(BigDecimal('1')).should == [-2, 0]    
  end

  it "Can be reversed with * and +" do
    # Example taken from BigDecimal documentation
    a = BigDecimal.new("42")
    b = BigDecimal.new("9")
    q,m = a.divmod(b)
    c = q * b + m
    a.should == c
  end

  it "raises TypeError if the argument cannot be coerced to BigDecimal" do
    lambda {
      @one.divmod('1')
    }.should raise_error(TypeError)
  end

end

