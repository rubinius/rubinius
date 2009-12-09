require File.dirname(__FILE__) + '/../../spec_helper'
require 'tempfile'

describe "Tempfile#path" do
  it "returns the path to the tempfile" do
    @tempfile = Tempfile.new("specs", tmp(""))
    tmpdir = tmp("")
    @tempfile.path[0, tmpdir.length].should == tmpdir
    @tempfile.path.should include("specs")
  end
end
