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
    if ENV['HOME']
    Dir.chdir(ENV['HOME'])
    home = Dir.pwd

    Dir.chdir
    Dir.pwd.should == home
    end
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
    lambda { Dir.chdir DirSpecs.nonexistent }.should raise_error(SystemCallError)
    lambda { Dir.chdir(DirSpecs.nonexistent) { } }.should raise_error(SystemCallError)
  end

  it "raises a SystemCallError if the original directory no longer exists" do
    dir1 = tmp('/testdir1')
    dir2 = tmp('/testdir2')
    File.exist?(dir1).should == false
    File.exist?(dir2).should == false
    Dir.mkdir dir1
    Dir.mkdir dir2
    begin
      lambda {
        Dir.chdir dir1 do
          Dir.chdir(dir2) { Dir.unlink dir1 }
        end
      }.should raise_error(SystemCallError)
    ensure
      Dir.unlink dir1 if File.exist?(dir1)
      Dir.unlink dir2 if File.exist?(dir2)
    end
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
