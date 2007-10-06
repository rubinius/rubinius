require File.dirname(__FILE__) + '/../../spec_helper'

@proc_call = shared "Proc#call" do |cmd|
  describe "Proc##{cmd}" do
    it "when called on a Proc created with Proc.new, it silently ignores extra arguments" do
      Proc.new { |a, b| a + b }.send(cmd, 1, 2, 5).should == 3
    end

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
  end
end

describe "Proc#call" do
  it_behaves_like(@proc_call, :call)
end
