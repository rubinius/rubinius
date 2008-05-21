require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#print" do
  before(:each) do
    @io = StringIO.new('')
  end
  after(:each) do
    $\ = nil
  end

  it "prints multiple items to the output" do
    @io.print(5,6,7,8).should == nil
    @io.string.should == '5678'
  end

  it "honors the output record separator global" do
    $\ = 'x'
    @io.print(5,6,7,8).should == nil
    @io.string.should == '5678x'
  end
end
