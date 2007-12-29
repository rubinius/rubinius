require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/shared/closed'

describe "Dir#path" do
  it "returns the path that was supplied to .new or .open" do
    dir = Dir.open DirSpecs.mock_dir
    dir.path.should == DirSpecs.mock_dir
    dir.close rescue nil
  end
end

describe "Dir#path" do
  it_behaves_like :dir_closed, :path
end
