require File.dirname(__FILE__) + '/../../spec_helper'

@proc_call = shared "Proc#call" do |cmd|
  describe "Proc##{cmd}" do
    it "invokes self" do
      Proc.new { "test!" }.send(cmd).should == "test!"
      lambda { "test!" }.send(cmd).should == "test!"
      proc { "test!" }.send(cmd).should == "test!"
    end
    
    it "sets self's parameters to the given values" do
      Proc.new { |a, b| a + b }.send(cmd, 1, 2).should == 3
      Proc.new { |*args| args }.send(cmd, 1, 2, 3, 4).should == [1, 2, 3, 4]
      Proc.new { |_, *args| args }.send(cmd, 1, 2, 3).should == [2, 3]
      
      lambda { |a, b| a + b }.send(cmd, 1, 2).should == 3
      lambda { |*args| args }.send(cmd, 1, 2, 3, 4).should == [1, 2, 3, 4]
      lambda { |_, *args| args }.send(cmd, 1, 2, 3).should == [2, 3]

      proc { |a, b| a + b }.send(cmd, 1, 2).should == 3
      proc { |*args| args }.send(cmd, 1, 2, 3, 4).should == [1, 2, 3, 4]
      proc { |_, *args| args }.send(cmd, 1, 2, 3).should == [2, 3]
    end
    
    it "sets self's single parameter to an Array of all given values" do
      [Proc.new { |x| [x] }, lambda { |x| [x] }, proc { |x| [x] }].each do |p|
        a = p.send(cmd)
        a.class.should == Array
        a.should == [nil]
        
        a = p.send(cmd, 1)
        a.class.should == Array
        a.should == [1]
        
        a = p.send(cmd, 1, 2)
        a.class.should == Array
        a.should == [[1, 2]]
        
        a = p.send(cmd, 1, 2, 3)
        a.class.should == Array
        a.should == [[1, 2, 3]]
      end
    end
    
    it "replaces missing arguments with nil when called on a Proc created with Proc.new" do
      Proc.new { |a, b| [a, b] }.send(cmd).should == [nil, nil]
      Proc.new { |a, b| [a, b] }.send(cmd, 1).should == [1, nil]
    end
    
    it "silently ignores extra arguments when called on a Proc created with Proc.new" do
      Proc.new { |a, b| a + b }.send(cmd, 1, 2, 5).should == 3
    end
    
    it "raises ArgumentError when called with too few arguments on a Proc created with Kernel#lambda or Kernel#proc" do
      should_raise(ArgumentError) { lambda { |a, b| [a, b] }.send(cmd) }
      should_raise(ArgumentError) { lambda { |a, b| [a, b] }.send(cmd, 1) }
      should_raise(ArgumentError) { proc { |a, b| [a, b] }.send(cmd) }
      should_raise(ArgumentError) { proc { |a, b| [a, b] }.send(cmd, 1) }
    end
    
    it "raises ArgumentError when called with too many arguments on a Proc created with Kernel#lambda or Kernel#proc" do
      should_raise(ArgumentError) { lambda { |a, b| [a, b] }.send(cmd, 1, 2, 3) }
      should_raise(ArgumentError) { proc { |a, b| [a, b] }.send(cmd, 1, 2, 3) }
    end
  end
end

describe "Proc#call" do
  it_behaves_like(@proc_call, :call)
end
