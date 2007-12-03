require File.dirname(__FILE__) + '/../../spec_helper'
require 'pathname'

describe "File.ftype" do
  
  # Identifies the type of the named file; the return string is one of ``file’’, 
  # `directory’’, ``characterSpecial’’, ``blockSpecial’’, ``fifo’’, ``link’’, 
  # `socket’’, or ``unknown’’.
  
  before :each do
    @file = "test.txt"
    @dir  = Dir.pwd
    File.open(@file, "w"){} # Touch
    #@char = Pathname.new(File.null).realpath

    platform :mswin do
      @block_dev = "NUL"
    end
    
    platform :not, :mswin do
      @fifo = "test_fifo"
      system("mkfifo #{@fifo}") unless File.exists?(@fifo)

      if File.exists?("/dev/fd0")
        @block = Pathname.new("/dev/fd0").realpath
      elsif File.exists?("/dev/diskette")
        @block = Pathname.new("/dev/diskette").realpath
      elsif File.exists?("/dev/cdrom")
        @block = Pathname.new("/dev/cdrom").realpath
      elsif File.exists?("/dev/sr0") # CDROM
        @block = Pathname.new("/dev/sr0").realpath
      elsif File.exists?("/dev/disk0")
        @block = "/dev/disk0"
      else
        @block = nil
        @link  = nil
      end
      
      if File.symlink?("/dev/fd0")
        @link = "/dev/fd0"
      elsif File.symlink?("/dev/diskette")
        @link = "/dev/diskette"
      elsif File.symlink?("/dev/cdrom")
        @link = "/dev/cdrom" 
      elsif File.symlink?("/dev/sr0")
        @link = "/dev/sr0" 
      else
        @link = "/tmp"
      end
      
    end
  end

  after :each do 
    File.delete(@fifo) if File.exist?(@fifo)
      
    @file   = nil
    @dir    = nil
    @char   = nil
    @block  = nil
    @fifo   = nil
    @link   = nil
    @socket = nil
  end

  it "return a string " do  
    File.ftype(@file).class.should == String
  end

  it "return 'file' when is a file" do
    File.ftype(@file).should == 'file'
  end

  it "return 'directory' when is a dir" do
    File.ftype(@dir).should == 'directory'
  end

  #   it "return characterSpecial when is a char"  do
  #      File.ftype(@char).should = 'characterSpecial'
  #   end

  it "return blockSpecial when is a block" do
    File.ftype(@block).should == 'blockSpecial'
  end

  it "return link when is a link" do
    File.ftype(@link).should == 'link'
  end

  it "return fifo when is a fifo" do
    File.ftype(@fifo).should == 'fifo'
  end   
  
  it "should return the type of the named file" do
    File.ftype(@file).should == "file"
    File.ftype("/dev/tty").should == "characterSpecial"
    File.ftype("/").should == "directory"
  end

  it "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
    lambda { File.ftype          }.should raise_error(ArgumentError)
    lambda { File.ftype('bogus') }.should raise_error(Errno::ENOENT)
  end
end 
