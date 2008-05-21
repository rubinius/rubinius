require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/symlink'

describe "File.symlink" do
  before :each do
    @file = "test.txt"
    @link = "test.lnk"
    File.delete(@link) if File.exist?(@link)
    File.delete(@file) if File.exist?(@file)
    File.open(@file,"w+")
  end

  after :each do
    File.unlink(@link) if File.exist?(@link)
    File.delete(@file) if File.exist?(@file)
    @link = nil
  end

  platform_is_not :windows do
    it "create a symlink between a source and target file" do
      File.symlink(@file, @link).should == 0
      File.exists?(@link).should == true
      File.identical?(@file, @link).should == true
    end

    it "create a symbolic link" do
      File.symlink(@file, @link)
      File.symlink?(@link).should == true
    end

    it "raises an Errno::EEXIST if the target already exists" do
      File.symlink(@file, @link)
      lambda { File.symlink(@file, @link) }.should raise_error(Errno::EEXIST)
    end

    it "raises an ArgumentError if not called with two arguments" do
      lambda { File.symlink        }.should raise_error(ArgumentError)
      lambda { File.symlink(@file) }.should raise_error(ArgumentError)
    end

    it "raises a TypeError if not called with String types" do
      lambda { File.symlink(@file, nil) }.should raise_error(TypeError)
      lambda { File.symlink(@file, 1)   }.should raise_error(TypeError)
      lambda { File.symlink(1, 1)       }.should raise_error(TypeError)
    end
  end
end

describe "File.symlink?" do
  it_behaves_like :file_symlink, :symlink?, File
end
