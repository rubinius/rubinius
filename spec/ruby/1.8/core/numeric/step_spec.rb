require File.dirname(__FILE__) + '/../../spec_helper'

# Invokes block with the sequence of numbers starting at num, incremented by 
# step on each call. The loop finishes when the value to be passed to the block 
# is greater than limit (if step is positive) or less than limit (if step is negative).
# If all the arguments are integers, the loop operates using an integer counter.
describe "Numeric#step" do 
  before(:each) do 
    @step  = 2
    @stepn = -2
    @stepnf = -2
    @stepf = 2.0
    @limitf = 10.0
    @limit = 10
    @base  = 1
    @basef = 5.4
  end  
  
  it "if base < limit > step then it should iterate (base-limit)/step times (integers)" do 
    x = []
    @base.step(@limit, @step) { |i| x << i }
    x.should == [1, 3, 5, 7, 9]  
  end
  
  it "iterate one time if step is bigger than base-limit (integers)" do 
    x = []
    @base.step(@limit, 11) { |i| x<< i }
    x.should == [1]    
  end
  
  it "not iterate if base is bigger than limit and step >0 (integers)" do 
    x = []
    12.step(@limit, @step) { |i| x<< i }
    x.should == []  
  end  
  
  it "iterate backward if base is bigger than limit (integers)" do 
    x = []
    10.step(1, @stepn) { |i| x << i}
    x.should == [10, 8, 6, 4, 2]  
  end  
  
  it "not iterate if base is minor than limit and step <0 (integers)" do 
    x = []
    res = @base.step(@limit, @stepn) { |i| x<< i }
    x.should == []
    res.should == @base
  end   
  
  it "if base < limit > step then it should iterate (base-limit)/step times (integers)" do 
    x = []
    @base.step(@limit, @step) { |i| x << i }
    x.should == [1, 3, 5, 7, 9]  
  end
  
  it "iterate one time if step is bigger than base-limit (integers)" do 
    x = []
    @base.step(@limit, 11) { |i| x<< i }
    x.should == [1]    
  end
      
  it "if base < limit > step then it should iterate (base-limit)/step times (floats)" do 
    x = []
    @basef.step(@limitf, @stepf) { |i| x << i }
    x.should == [5.4, 7.4, 9.4]  
  end
  
  it "iterate one time if step is bigger than base-limit (floats)" do 
    x = []
    @basef.step(@limitf, 11) { |i| x<< i }
    x.should == [5.4]
  end
  
  it "not iterate if base is bigger than limit and step >0 (floats)" do 
    x = []
    res = 12.0.step(@limitf, @stepf) { |i| x<< i }
    x.should == []
    res.should == 12.0
  end  
  
  it "iterate backward if base is bigger than limit (floats)" do 
    x = []
    res = 10.0.step(1.0, @stepnf) { |i| x << i}
    x.should == [10, 8, 6, 4, 2]
    res.should == 10.0
  end  
  
  it "not iterate if base is minor than limit and step <0 (floats)" do 
    x = []
    res = @basef.step(@limitf, @stepnf) { |i| x<< i }
    x.should == []
    res.should == @basef
  end   
  
  it "if base < limit > step then iterate (base-limit)/step times (floats)" do 
    x = []
    @basef.step(@limitf, @stepf) { |i| x << i }
    x.should == [5.4, 7.4, 9.4]
  end
  
  it "raises an ArgumentError if not passed not passed Numeric types in the correct range" do
    lambda { @base.step            }.should raise_error(ArgumentError)
    lambda { @base.step(100,0)     }.should raise_error(ArgumentError)
    lambda { @base.step(nil)       }.should raise_error(ArgumentError)
    lambda { @base.step('test')    }.should raise_error(ArgumentError)
    lambda { @base.step(true, 123) }.should raise_error(ArgumentError)
  end
  
  it "raises a LocalJumpError if not provided a block" do
    lambda { @base.step(5, 5)      }.should raise_error(LocalJumpError)
    lambda { @base.step(5, 3.4)    }.should raise_error(LocalJumpError)
    lambda { @base.step(5.0, 2)    }.should raise_error(LocalJumpError)
    lambda { @base.step(5.0, 1.0)  }.should raise_error(LocalJumpError)
  end 
end
