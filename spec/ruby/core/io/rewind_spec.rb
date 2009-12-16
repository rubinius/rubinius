# -*- encoding: utf-8 -*-
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#rewind" do
  before :each do
    @file = File.open(File.dirname(__FILE__) + '/fixtures/readlines.txt', 'r')
    @io = IO.open @file.fileno, 'r'
  end

  after :each do
    # we *must* close both in order to not leak descriptors
    @io.close unless @io.closed?
    @file.close unless @file.closed? rescue Errno::EBADF
  end

  it "positions the instance to the beginning of input" do
    @io.readline.should == "Voici la ligne une.\n"
    @io.readline.should == "Qui è la linea due.\n"
    @io.rewind
    @io.readline.should == "Voici la ligne une.\n"
  end

  it "positions the instance to the beginning of input and clears EOF" do
    value = @io.read
    @io.rewind
    @io.eof?.should == false
    value.should == @io.read
  end

  it "sets lineno to 0" do
    @io.readline.should == "Voici la ligne une.\n"
    @io.lineno.should == 1
    @io.rewind
    @io.lineno.should == 0
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.rewind }.should raise_error(IOError)
  end
end
