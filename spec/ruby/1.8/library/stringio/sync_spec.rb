require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#sync" do
  before(:each) do
    @io = StringIO.new('')
  end

  it "is true" do
    @io.sync.should == true
  end
end

describe "StringIO#sync=" do
  before(:each) do
    @io = StringIO.new('')
  end

  it "does not change 'sync' status" do
    @io.sync = false
    @io.sync.should == true
  end
end
