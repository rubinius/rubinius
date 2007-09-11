require File.dirname(__FILE__) + '/../../spec_helper'
failure :rbx do
  require 'pathname'
end

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
        @link  = "/dev/fd0" if File.symlink?("/dev/fd0")
      elsif File.exists?("/dev/diskette")
        @block = Pathname.new("/dev/diskette").realpath
        @link  = "/dev/diskette" if File.symlink?("/dev/diskette")
      elsif File.exists?("/dev/cdrom")
        @block = Pathname.new("/dev/cdrom").realpath
        @link  = "/dev/cdrom" if File.symlink?("/dev/cdrom")
      elsif File.exists?("/dev/sr0") # CDROM
        @block = Pathname.new("/dev/sr0").realpath
        @link  = "/dev/sr0" if File.symlink?("/dev/sr0") 
      elsif File.exists?("/dev/disk0")
        @block = "/dev/disk0"
        @link  = "/tmp"
      else
        @block = nil
        @link  = nil
      end
    end
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
    should_raise(ArgumentError){ File.ftype }
    should_raise(Errno::ENOENT){ File.ftype('bogus') }
  end

  after :each do 
    File.delete(@fifo)
      
    @file   = nil
    @dir    = nil
    @char   = nil
    @block  = nil
    @fifo   = nil
    @link   = nil
    @socket = nil
  end
end 
