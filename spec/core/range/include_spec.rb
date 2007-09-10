require File.dirname(__FILE__) + '/../../spec_helper'

@range_include = shared "Range#include?" do |cmd|
  describe "Range##{cmd}" do
    it "returns true if other is an element" do
      ((0..5) === 2).should == true
      ((-5..5) === 0).should == true
      ((-1...1) === 10.5).should == false
      ((-10..-2) === -2.5).should == true
      (('C'..'X') === 'M').should == true
      (('C'..'X') === 'A').should == false
      (('B'...'W') === 'W').should == false
      (('B'...'W') === 'Q').should == true
      ((0xffff..0xfffff) === 0xffffd).should == true
      ((0xffff..0xfffff) === 0xfffd).should == false
      ((0.5..2.4) === 2).should == true
      ((0.5..2.4) === 2.5).should == false
      ((0.5..2.4) === 2.4).should == true
      ((0.5...2.4) === 2.4).should == false
    end
  end
end

describe "Range#include?" do
  it_behaves_like(@range_include, :include?)
end
