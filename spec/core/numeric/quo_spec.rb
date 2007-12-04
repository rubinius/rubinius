require File.dirname(__FILE__) + '/../../spec_helper'

# Equivalent to Numeric#/, but overridden in subclasses.
describe "Numeric#quo" do  
  specify "quo should return the floating-point result of self divided by other" do
    # the to_f is required because RSpec (I'm assuming) requires 'rational'
    2.quo(2.5).to_s.should == '0.8'
    5.quo(2).to_f.to_s.should == '2.5' 
  end

  specify "quo should NOT raise an exception when other is zero" do
    # 1.quo(0) should also not raise (i.e works in irb and from a file),
    # but fails here.
    1.quo(0.0).to_s.should == 'Infinity'
    1.quo(-0.0).to_s.should == '-Infinity'
  end

  it "quo right  integers" do 
    13.quo(4).should be_close(3.25, TOLERANCE)
    4.quo(13).should be_close( 0.307692307692308, TOLERANCE)
  end
     
  it "quo right  integers and floats" do 
    13.quo(4.0).should be_close(3.25, TOLERANCE)
    4.quo(13).should be_close(0.307692307692308, TOLERANCE)
  end
  
  it "quo right the integers and floats" do 
    13.quo(4.0).should be_close(3.25, TOLERANCE)
    4.quo(13).should be_close(0.307692307692308, TOLERANCE)
  end
  
  it "quo right  floats" do 
    13.0.quo(4.0).should be_close(3.25, TOLERANCE)
    4.0.quo(13).should be_close(0.307692307692308, TOLERANCE)
  end
   
  # NOTE: Doesn't work when run with RSpec because it loads rational.rb
  it " should quo right with bignums and integers" do
    2147483648.quo( 100).to_s.should == 21474836.48.to_s
  end
  
  # NOTE: Doesn't work when run with RSpec because it loads rational.rb
  it "not raise a Exception when quo by 0" do
    13.quo(0)
    13.0.quo(0)
    3**33.quo(0)
  end
  
  # NOTE: Doesn't work when run with RSpec because it loads rational.rb
  it "raise the expected exception" do
    lambda { 13.quo         }.should raise_error(ArgumentError)
    lambda { 13.quo(nil)    }.should raise_error(TypeError)
    lambda { 13.quo('test') }.should raise_error(TypeError)
    lambda { 13.quo(true)   }.should raise_error(TypeError)   
  end 
end
