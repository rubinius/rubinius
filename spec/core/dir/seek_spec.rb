require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "Dir#seek" do
  before(:each) do
    @dir = Dir.open mock_dir
  end
  
  after(:each) do
    @dir.close
  end
  
  it "moves the current read position" do
    pos = @dir.pos
    a   = @dir.read
    b   = @dir.read
    ret = @dir.seek pos
    c   = @dir.read
  
    a.should_not == b
    b.should_not == c
    c.should     == a
  end
  
  it "returns the Dir instance" do
    @dir.seek(0).should == @dir
  end
end
