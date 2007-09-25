require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/close_spec'

describe "Dir#path" do
  it "returns the path that was supplied to .new or .open" do
    dir = Dir.open mock_dir
    dir.path.should == mock_dir
    dir.close rescue nil
  end
end

describe "Dir#path" do
  it_behaves_like @dir_closed, :path
end
