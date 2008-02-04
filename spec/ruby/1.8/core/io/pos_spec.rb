require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#pos" do

  before :each do
    @fname = 'test.txt'
    File.open @fname, 'w' do |f| f.write "123" end
  end

  after :each do
    File.unlink @fname
  end

  it "gets the offset" do
    File.open @fname do |f|
      f.pos.should == 0
      f.read 1
      f.pos.should == 1
      f.read 2
      f.pos.should == 3
    end
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.pos }.should raise_error(IOError)
  end

end

describe "IO#pos=" do

  before :each do
    @fname = 'test.txt'
    File.open @fname, 'w' do |f| f.write "123" end
  end

  after :each do
    File.unlink @fname
  end

  it "sets the offset" do
    File.open @fname do |f|
      val1 = f.read 1
      f.pos = 0
      f.read(1).should == val1
    end
  end

  it "can handle any numerical argument without breaking" do
    File.open @fname do |f|
      f.seek(1.2).should == 0
      f.seek(2**32).should == 0
      f.seek(1.23423423432e12).should == 0
      f.seek(0.00000000000000000000001).should == 0
      lambda { f.seek(2**128) }.should raise_error(RangeError)
    end
  end
  
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.pos = 0 }.should raise_error(IOError)
  end

end

