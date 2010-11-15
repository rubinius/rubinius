require File.expand_path('../../../spec_helper', __FILE__)

module Specs
  module Float
    class CanCoerce
      def initialize(a)
        @a = a
      end

      def coerce(b)
        [self.class.new(b), @a]
      end

      def /(b)
        @a.to_i % b.to_i
      end
    end
  end
end

describe "Float#/" do
  it "returns self divided by other" do
    (5.75 / -2).should be_close(-2.875,TOLERANCE)
    (451.0 / 9.3).should be_close(48.494623655914,TOLERANCE)
    (91.1 / -0xffffffff).should be_close(-2.12108716418061e-08, TOLERANCE)
  end
  
  it "properly coerces objects" do
    (5.0 / Specs::Float::CanCoerce.new(5)).should be_close(0, TOLERANCE)
  end
  
  it "properly handles BigDecimal argument" do
    require 'bigdecimal'
    (2.0 / BigDecimal.new('5.0')).should be_close(0.4, TOLERANCE)
    (2.0 / BigDecimal.new('Infinity')).should == 0
    (2.0 / BigDecimal.new('-Infinity')).should == 0
    (2.0 / BigDecimal.new('0.0')).infinite?.should == 1
    (2.0 / BigDecimal.new('-0.0')).infinite?.should == -1
    (2.0 / BigDecimal.new('NaN')).nan?.should == true
  end
  
  it "does NOT raise ZeroDivisionError if other is zero" do
    (1.0 / 0.0).to_s.should == 'Infinity'
    (-1.0 / 0.0).to_s.should == '-Infinity'
    (1.0 / -0.0).to_s.should == '-Infinity'
    (-1.0 / -0.0).to_s.should == 'Infinity'
    (0.0 / 0.0).to_s.should == 'NaN'
    (-0.0 / 0.0).to_s.should == 'NaN'
    (-0.0 / -0.0).to_s.should == 'NaN'
  end
end
