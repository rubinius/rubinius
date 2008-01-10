require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#gets" do
  before(:each) do
    @io = StringIO.new("g e t s")
  end

  after(:each) do
    $/ = "\n"
  end

  it "returns the next 'line'" do
    @io.gets.should == 'g e t s'
    @io.gets.should == nil
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
end
