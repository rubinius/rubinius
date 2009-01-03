require File.dirname(__FILE__) + '/../spec_helper'

describe "Kernel#raise" do
  it "aborts the execution" do
    some_var = "This should not change"
    
    lambda {
      raise "Stops the execution"
      some_var = "Ha, it was changed!"
    }.should raise_error(RuntimeError)
    
    some_var.should == "This should not change"
  end
end

describe "Kernel#raise inside a rescue block" do
  it "re-raises the exception that was rescued" do
    lambda {
      begin
        raise "This exception is re-raised"
      rescue
        raise
      end
    }.should raise_error(RuntimeError, "This exception is re-raised")
  end
  
  it "ignores inner exceptions" do
    lambda {
      begin
        raise "This exception is re-raised"
      rescue
        begin
          raise "This is another error"
        rescue
        end
        
        raise
      end
    }.should raise_error(RuntimeError, "This exception is re-raised")
  end
end