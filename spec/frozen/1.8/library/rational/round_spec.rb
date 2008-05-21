require File.dirname(__FILE__) + '/../../spec_helper'
require 'rational'

# Rounds num to the nearest integer
describe "Rational#round" do  
  it " round (down) " do 
    Rational(   0,  1).round.should ==  0.0
    Rational( 101,100).round.should ==  1
    Rational(-101,100).round.should == -1
  end  
  
  it " round (up) " do  
    Rational( 191,100).round.should ==  2
    Rational(-191,100).round.should == -2
  end  
  
end
