require "#{File.dirname(__FILE__)}/../../spec_helper"
require "#{File.dirname(__FILE__)}/fixtures/file_types.rb"

describe "File.ftype" do

  it "raises ArgumentError if not given exactly one filename" do
    lambda { File.ftype }.should raise_error(ArgumentError)
    lambda { File.ftype('blah', 'bleh') }.should raise_error(ArgumentError)
  end
  
  it "raises Errno::ENOENT if the file is not valid" do
    l = lambda { File.ftype("/#{$$}#{Time.now.to_f}#{$0}") }
    l.should raise_error(Errno::ENOENT)
  end

  it "returns a String " do  
    FileSpecs.normal_file do |file|
      File.ftype(file).class.should == String
    end
  end

  it "returns 'file' when is a file" do
    FileSpecs.normal_file do |file|
      File.ftype(file).should == 'file'
    end
  end

  it "returns 'directory' when is a dir" do
    FileSpecs.directory do |dir|
      File.ftype(dir).should == 'directory'
    end
  end

  it "returns 'characterSpecial' when is a char"  do
    FileSpecs.character_device do |char|
      File.ftype(char).should == 'characterSpecial'
    end
  end

  platform_is_not :freebsd do  # FreeBSD does not have block devices
    it "returns 'blockSpecial' when is a block" do
      FileSpecs.block_device do |block|
        File.ftype(block).should == 'blockSpecial'
      end
    end
  end

  it "returns 'link' when is a link" do
    FileSpecs.symlink do |link|
      File.ftype(link).should == 'link'
    end
  end

  it "returns fifo when is a fifo" do
    FileSpecs.fifo do |fifo|
      File.ftype(fifo).should == 'fifo'
    end
  end   
end 
