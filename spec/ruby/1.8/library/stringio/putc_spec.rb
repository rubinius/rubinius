require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#putc" do
  before(:each) do
    @io = StringIO.new('')
  end

  it "handles characters and character codes" do
    @io.putc(65).should == 65
    @io.putc('A').should == 'A'
    @io.putc('AA').should == 'AA'
    @io.string.should == 'AAA'
  end
end
