require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#putc" do
  before(:each) do
    @io = StringIO.new('')
  end

  it "handles characters and character codes" do
    @io.pos.should == 0

    @io.putc(65).should == 65
    @io.pos.should == 1

    @io.putc('B').should == 'B'
    @io.pos.should == 2

    @io.putc('CD').should == 'CD'
    @io.pos.should == 3

    @io.string.should == 'ABC'
  end
end
