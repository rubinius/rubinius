require File.dirname(__FILE__) + '/../../spec_helper'

# Rounds num to the nearest integer
describe "Numeric#round" do  
  it " round (down) " do 
    0.round.should == 0.0
    100.round.should == 100
    -100.round.should == -100
  end  
  
  it " round (up) " do  
    34.56.round.should == 35
    -34.56.round.should == -35
  end  
  
  it " round twos complement " do
    9223372036854775808.round.should == 9223372036854775808 
    -9223372036854775808.round.should == -9223372036854775808 
  end   
end
