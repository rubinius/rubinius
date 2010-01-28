# -*- encoding: utf-8 -*-
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/set_pos'

describe "IO#seek" do
  it_behaves_like(:io_set_pos, :seek)

  before :each do
    @file = File.open(File.dirname(__FILE__) + '/fixtures/readlines.txt', 'r')
    @io = IO.open @file.fileno, 'r'
  end

  after :each do
    # we *must* close both in order to not leak descriptors
    @io.close unless @io.closed?
    @file.close unless @file.closed? rescue Errno::EBADF
  end

  it "moves the read position relative to the current position with SEEK_CUR" do
    lambda { @io.seek(-1) }.should raise_error(Errno::EINVAL)
    @io.seek(10, IO::SEEK_CUR)
    @io.readline.should == "igne une.\n"
    @io.seek(-5, IO::SEEK_CUR)
    @io.readline.should == "une.\n"
  end

  it "moves the read position relative to the start with SEEK_SET" do
    @io.seek(1)
    @io.pos.should == 1
    @io.rewind
    @io.seek(42, IO::SEEK_SET)
    @io.readline.should == "quí está la línea tres.\n"
    @io.seek(5, IO::SEEK_SET)
    @io.readline.should == " la ligne une.\n"
  end

  it "moves the read position relative to the end with SEEK_END" do
    @io.seek(0, IO::SEEK_END)
    @io.tell.should == 134
    @io.seek(-25, IO::SEEK_END)
    @io.readline.should == "cinco.\n"
  end

  it "moves the read position and clears EOF with SEEK_SET" do
    value = @io.read
    @io.seek(0, IO::SEEK_SET)
    @io.eof?.should == false
    value.should == @io.read
  end

  it "moves the read position and clears EOF with SEEK_CUR" do
    value = @io.read
    @io.seek(-1, IO::SEEK_CUR)
    @io.eof?.should == false
    value[-1].should == @io.read[0]
  end

  it "moves the read position and clears EOF with SEEK_END" do
    value = @io.read
    @io.seek(-1, IO::SEEK_END)
    @io.eof?.should == false
    value[-1].should == @io.read[0]
  end
end
