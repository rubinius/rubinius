require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/close_spec'

@dir_pos = shared "Dir#pos" do |cmd|
  describe "Dir##{cmd}" do
    specify 'Both Dir#pos and Dir#tell give the current dir position' do
      @dir = Dir.open mock_dir

      @dir.pos.should == 1
      @dir.tell.should == 2
      @dir.pos.should == 3
      @dir.tell.should == 4

      @dir.close rescue nil
    end

    specify 'Dir#pos= also seeks to a certain position but returns the position number instead' do
      @dir = Dir.open mock_dir

      pos = @dir.pos
      a   = @dir.read
      b   = @dir.read
      ret = @dir.pos = pos
      c   = @dir.read
  
      a.should_not == b
      b.should_not == c
      c.should     == a

      ret.should == pos

      @dir.close rescue nil
    end
  end
end

describe "Dir#pos" do
  it_behaves_like @dir_closed, :pos
end
