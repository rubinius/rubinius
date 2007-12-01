require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "Dir.chdir" do
  before(:each) do
    @original = Dir.pwd
  end
  
  after(:each) do
    Dir.chdir(@original)
  end
  
  it "defaults to $HOME with no arguments" do
    Dir.chdir
    Dir.pwd.should == ENV['HOME'] 
  end
  
  it "changes to the specified directory" do
    Dir.chdir DirSpecs.mock_dir
    Dir.pwd.should == DirSpecs.mock_dir
  end
  
  it "returns 0 when successfully changing directory" do
    Dir.chdir(@original).should == 0
  end
  
  it "returns the value of the block when a block is given" do
    Dir.chdir(@original) { :block_value }.should == :block_value
  end
  
  it "changes to the specified directory for the duration of the block" do
    ar = Dir.chdir(DirSpecs.mock_dir) { |dir| [dir, Dir.pwd] }
    ar.should == [DirSpecs.mock_dir, DirSpecs.mock_dir]

    Dir.pwd.should == @original
  end
  
  it "raises a SystemCallError if the directory does not exist" do
    should_raise(SystemCallError) { Dir.chdir DirSpecs.nonexistent }
  end

  it "always returns to the original directory when given a block" do
    begin
      Dir.chdir(DirSpecs.mock_dir) do
        raise StandardError, "something bad happened"
      end
    rescue StandardError
    end

    Dir.pwd.should == @original
  end
end
