require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/close_spec'

describe "Dir#read" do
  it "returns the file name in the current seek position" do
    dir = Dir.open mock_dir
    dir.read.should == '.'
    dir.close
  end
end

describe "Dir#read" do
  it_behaves_like @dir_closed, :read
end
