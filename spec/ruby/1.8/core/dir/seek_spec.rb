require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "Dir#seek" do
  before(:each) do
    @dir = Dir.open DirSpecs.mock_dir
  end

  after(:each) do
    @dir.close
  end

  it "moves the read position to a previously obtained position" do
    pos = @dir.pos
    a   = @dir.read
    b   = @dir.read
    ret = @dir.pos = pos
    c   = @dir.read

    a.should_not == b
    b.should_not == c
    c.should     == a
  end

  it "does nothing if given a position that was not previously returned by #pos/#tell" do
    a = @dir.read
    b = @dir.read

    @dir.rewind
    @dir.seek 5

    c = @dir.read

    a.should == c
    a.should_not == b
  end

  it "returns the Dir instance" do
    @dir.seek(@dir.pos).should == @dir
  end
end
