require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#sync=" do
  before :each do
    @file = File.open(File.dirname(__FILE__) + '/fixtures/readlines.txt', 'r')
    @io = IO.open @file.fileno, 'r'
  end

  after :each do
    @file.close unless @file.closed?
  end

  it "sets the sync mode to true or false" do
    @io.sync = true
    @io.sync.should == true
    @io.sync = false
    @io.sync.should == false
  end

  it "accepts non-boolean arguments" do
    @io.sync = 10
    @io.sync.should == true
    @io.sync = nil
    @io.sync.should == false
    @io.sync = Object.new
    @io.sync.should == true
  end

  it "raises IOError on closed stream" do
    @file.close
    lambda { @file.sync = true }.should raise_error(IOError)
  end
end

describe "IO#sync" do
  before :each do
    @file = File.open(File.dirname(__FILE__) + '/fixtures/readlines.txt', 'r')
    @io = IO.open @file.fileno, 'r'
  end

  after :each do
    @file.close unless @file.closed?
  end

  it "returns the current sync mode" do
    @io.sync.should == false
  end

  it "raises IOError on closed stream" do
    @file.close
    lambda { @file.sync }.should raise_error(IOError)
  end
end
