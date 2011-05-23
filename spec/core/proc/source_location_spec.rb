require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/procs', __FILE__)

describe "Proc#source_location" do
  describe "on a proc object" do
    before(:each) do
      @proc = ProcSpecs::SourceLocation.simple_proc
    end

    it "returns an Array" do
      @proc.source_location.should be_an_instance_of(Array)
    end

    it "sets the first value to the path of the file in which the method was defined" do
      file = @proc.source_location.first
      file.should be_an_instance_of(String)
      file.should == File.dirname(__FILE__) + '/fixtures/procs.rb'
    end

    it "sets the last value to a Fixnum representing the line on which the method was defined" do
      line = @proc.source_location.last
      line.should be_an_instance_of(Fixnum)
      line.should == 4
    end
  end

  describe "on a block passed to a method" do
    before(:each) do
      @block = ProcSpecs::SourceLocation.stashed_block
    end

    it "returns an Array" do
      @block.source_location.should be_an_instance_of(Array)
    end

    it "sets the first value to the path of the file in which the method was defined" do
      file = @block.source_location.first
      file.should be_an_instance_of(String)
      file.should == File.dirname(__FILE__) + '/fixtures/procs.rb'
    end

    it "sets the last value to a Fixnum representing the line on which the method was defined" do
      line = @block.source_location.last
      line.should be_an_instance_of(Fixnum)
      line.should == 14
    end
  end
end
