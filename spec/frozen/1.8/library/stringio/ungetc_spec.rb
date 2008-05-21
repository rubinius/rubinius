require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#ungetc" do
  before(:each) do
    @io = StringIO.new('1234')
  end

  it "writes a character before the current position" do
    @io.read(1)
    @io.ungetc(65)
    @io.string.should == 'A234'
  end

  it "rewinds the current position by one" do
    @io.read(2)
    @io.pos.should == 2
    @io.ungetc(65)
    @io.pos.should == 1
  end

  it "does nothing when pos == 0" do
    @io.ungetc(65)
    @io.string.should == '1234'
  end

  it "does not accept strings" do
    lambda { @io.ungetc('A') }.should raise_error(TypeError)
  end
end
