require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/shared/closed'

describe "Dir#read" do
  before :all do
    DirSpecs.create_mock_dirs
  end

  after :all do
    DirSpecs.delete_mock_dirs
  end

  it "returns the file name in the current seek position" do
    # an FS does not necessarily impose order
    ls = Dir.entries DirSpecs.mock_dir
    dir = Dir.open DirSpecs.mock_dir
    ls.should include(dir.read)
    dir.close
  end

  it_behaves_like :dir_closed, :read
end
