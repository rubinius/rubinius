require File.dirname(__FILE__) + '/../../spec_helper'

@proc_call = shared "Proc#call" do |cmd|
  describe "Proc##{cmd} when called on a Proc created with Kernel.proc" do
    it "raises an ArgumentError when given to many arguments" do
      should_raise(ArgumentError, "wrong number of arguments (3 for 2)") do
        Kernel.proc { |a, b| a + b }.send(cmd, 1, 2, 5)
      end
    end
  end
  
  describe "Proc#{cmd} when called on a Proc created with Proc.new" do
    it "silently ignores extra arguments" do
      Proc.new { |a, b| a + b }.send(cmd, 1, 2, 5).should == 3
    end
  end
  
  describe "Proc#{cmd}" do
    it "invokes self" do
      lambda { "test!" }.send(cmd).should == "test!"
    end
    
    it "sets self's parameters to the given values" do
      lambda { |a, b| a + b }.send(cmd, 1, 2).should == 3
      lambda { |*args| args }.send(cmd, 1, 2, 3, 4).should == [1, 2, 3, 4]
      lambda { |_, *args| args }.send(cmd, 1, 2, 3).should == [2, 3]
    end
  end
end

describe "Proc#call" do
  it_behaves_like(@proc_call, :call)
end