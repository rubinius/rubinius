require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#gets" do
  before(:each) do
    @io = StringIO.new("g e t s")
    @io_para = StringIO.new("para1-1\npara1-2\n\n\npara2-1\npara2-2\n\n\n\n")
  end

  after(:each) do
    $/ = "\n"
  end

  it "returns the next 'line'" do
    @io.gets.should == 'g e t s'
  end

  it "returns nil at the end" do
    @io.gets
    @io.gets.should be_nil
  end

  it "raises an IOError when it is not open for reading" do
    @io.close_read
    lambda { @io.gets }.should raise_error(IOError)
  end

  it "supports separator strings" do
    @io.gets('e').should == 'g e'
    @io.gets('e').should == ' t s'
  end

  it "honors the $/ global separator" do
    $/ = ' '
    @io.gets.should == 'g '
    @io.gets.should == 'e '
    @io.gets.should == 't '
    @io.gets.should == 's'
  end

  it "returns the next paragrah when separator is an empty string" do
    @io_para.gets("").should == "para1-1\npara1-2\n"
    @io_para.gets("").should == "para2-1\npara2-2\n"
    @io_para.gets("").should be_nil
    StringIO.new("\n\n\n\n\n\n\n\n\n").gets("").should be_nil
  end

  it "returns the entire content if separator is nil" do
    @io_para.gets(nil).should == @io_para.string
    @io_para.gets(nil).should be_nil
  end
end
