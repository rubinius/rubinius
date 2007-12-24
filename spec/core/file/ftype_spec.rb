require File.dirname(__FILE__) + '/../../spec_helper'
require 'pathname'

describe "File.ftype" do
  
  # Identifies the type of the named file; the return string is one of ``file’’, 
  # `directory’’, ``characterSpecial’’, ``blockSpecial’’, ``fifo’’, ``link’’, 
  # `socket’’, or ``unknown’’.
  
  before :all do
    @file = "test.txt"
    @dir  = Dir.pwd
    File.open(@file, "w"){} # Touch

    platform :mswin do
      @block_dev = "NUL"
    end
    
    platform_not :mswin do
      @fifo = "test_fifo"
      system("mkfifo #{@fifo}") unless File.exists?(@fifo)

      # Block devices
      @block = `find /dev -type b 2> /dev/null`.split("\n").first
      @block = Pathname.new(@block).realpath if @block

      # Character devices
      @char = `find /dev -type c 2> /dev/null`.split("\n").first
      @path = Pathname.new(@path).realpath if @path
      
      # Symlinks
      %w[/dev /usr/bin /usr/local/bin].each do |dir|
        links = `find /usr/local/bin -type l 2> /dev/null`.split("\n")
        next if links.empty?
        @link = links.first
        break
      end
    end
  end

  after :all do 
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

  it "return characterSpecial when is a char"  do
    File.ftype(@char).should == 'characterSpecial'
  end

  platform_not :freebsd do  # FreeBSD does not have block devices
    it "return blockSpecial when is a block" do
      File.ftype(@block).should == 'blockSpecial'
    end
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
