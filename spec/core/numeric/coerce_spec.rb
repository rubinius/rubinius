require File.dirname(__FILE__) + '/../../spec_helper'

describe "Numeric#coerce" do   
    class String
      def coerce(other)
        case other
        when Integer
          begin
            return other, Integer(self)
          rescue
            return Float(other), Float(self)
          end
        when Float
          return other, Float(self)
        else
          super
        end
      end
    end
    
  before(:each) do 
    @integer = 1
    @float   = 2.5    
  end  
  
  it "coerce integers" do 
    @integer.coerce(@integer).should == [1,1]
    @integer.coerce(@float).should ==[2.5,1]
  end  
  
  it "coerce floats" do 
    @float.coerce(@float).should == [2.5,2.5]
    @float.coerce(@integer).should == [1,2.5]
  end  
  
  it "coerce with 0" do 
    @integer.coerce(0).should == [0,1]
    @float.coerce(0).should == [0,2.5]
  end  
  
  it "coerce with bignums " do 
    @integer.coerce(4294967296).should == [4294967296, 1]
    @float.coerce(4294967296).should == [4294967296.0, 2.5]
  end  
 
  it "coerce strings to strings" do 
    3.should == 1 + "2"
    -1.3.should_be_close((1 - "2.3"), 0.001)
    3.5.should == 1.2 + "2.3"
    0.should == 1 - "1"
  end  
  
  it "return the vaule if number is different to 0" do 
    should_raise(ArgumentError){ @integer.coerce("test") }
    should_raise(TypeError){ @integer.coerce(nil) }
    should_raise(TypeError){ @integer.coerce(false) }      
    should_raise(ArgumentError){ @integer.coerce }
  end  
end
