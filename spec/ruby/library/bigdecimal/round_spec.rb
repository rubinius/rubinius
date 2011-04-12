require File.expand_path('../../../spec_helper', __FILE__)
require 'bigdecimal'

describe "BigDecimal#round" do
  ruby_bug "#4567", "1.9.3" do
    it "correctly rounds using ROUND_HALF_DOWN" do
      15.times do |n|
        x = BigDecimal.new("0.5#{'0'*n}1")
        x.round(0, BigDecimal::ROUND_HALF_DOWN).should == 1
        x = BigDecimal.new("-0.5#{'0'*n}1")
        x.round(0, BigDecimal::ROUND_HALF_DOWN).should == -1
        x = BigDecimal.new("5#{'0'*n}1")
        x.round(-(n+2), BigDecimal::ROUND_HALF_DOWN).should == 10**(n+2)
      end
    end

    it "correctly rounds using ROUND_HALF_EVEN" do
      15.times do |n|
        x = BigDecimal.new("0.5#{'0'*n}1")
        x.round(0, BigDecimal::ROUND_HALF_EVEN).should == 1
        x = BigDecimal.new("-0.5#{'0'*n}1")
        x.round(0, BigDecimal::ROUND_HALF_EVEN).should == -1
        x = BigDecimal.new("5#{'0'*n}1")
        x.round(-(n+2), BigDecimal::ROUND_HALF_EVEN).should == 10**(n+2)
      end
    end
  end
end

