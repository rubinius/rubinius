require File.dirname(__FILE__) + '/../../spec_helper'
# Do not change anything above this line

require 'fileutils'


describe "IO#dup" do
  before :all do
    @file = "/tmp/rubinius_spec_io_dup_#{$$}_#{Time.now.to_f}"
  end

  before :each do
    @f = File.open @file, 'w+'
    @i = @f.dup

    @f.sync = true
    @i.sync = true
  end

  after :each do
    @i.close rescue nil
    @f.close rescue nil
  end

  after :all do
    FileUtils.rm @file
  end

  it "returns a new IO instance" do
    @i.class.should == @f.class
  end

  it "sets a new descriptor on the returned object" do
    @i.fileno.should_not == @f.fileno
  end

  it "shares the original stream between the two IOs" do
    start = @f.pos
    @i.pos.should == start

    s =  "Hello, wo.. wait, where am I?\n"
    s2 = "<evil voice>       Muhahahaa!"

    @f.write s
    @i.pos.should == @f.pos

    @i.rewind
    @i.gets.should == s

    @i.rewind
    @i.write s2

    @f.rewind
    @f.gets.should == "#{s2}\n"
  end

  it "allows closing the new IO without affecting the original" do
    @i.close
    lambda { @f.gets }.should_not raise_error(Exception)

    @i.closed?.should == true
    @f.closed?.should == false
  end


  it "allows closing the original IO without affecting the new one" do
    @f.close
    lambda { @i.gets }.should_not raise_error(Exception)

    @i.closed?.should == false
    @f.closed?.should == true
  end
end
