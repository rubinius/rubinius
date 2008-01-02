require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/shared/closed'
require File.dirname(__FILE__) + '/shared/pos'

describe "Dir#pos" do
  it_behaves_like :dir_pos, :pos
end

describe "Dir#pos" do
  it_behaves_like :dir_closed, :pos
end

describe "Dir#pos=" do
  before(:each) do
    @dir = Dir.open DirSpecs.mock_dir
  end

  after(:each) do
    @dir.close
  end

  it "seeks to a certain previous position and returns an Integer position number" do
    pos = @dir.pos
    a   = @dir.read
    b   = @dir.read
    ret = @dir.pos = pos
    c   = @dir.read

    a.should_not == b
    b.should_not == c
    c.should     == a

    pos.should  be_kind_of(Integer)
    ret.should  be_kind_of(Integer)
  end

  it "does nothing if given a position that was not previously returned by #pos/#tell" do
    a = @dir.read
    b = @dir.read

    @dir.rewind
    @dir.pos = 5

    c = @dir.read

    a.should == c
    a.should_not == b
  end
end
