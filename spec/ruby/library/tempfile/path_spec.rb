require File.dirname(__FILE__) + '/../../spec_helper'
require 'tempfile'

describe "Tempfile#path" do
  before :each do
    @tempfile = Tempfile.new("specs", tmp(""))
  end

  after :each do
    @tempfile.unlink if @tempfile.path
  end

  it "returns the path to the tempfile" do
    tmpdir = tmp("")
    @tempfile.path[0, tmpdir.length].should == tmpdir
    @tempfile.path.should include("specs")
  end
end
