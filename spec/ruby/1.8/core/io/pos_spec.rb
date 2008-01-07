require File.dirname(__FILE__) + '/../../spec_helper'

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

end

