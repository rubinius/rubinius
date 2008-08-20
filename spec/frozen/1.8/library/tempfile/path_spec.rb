require File.dirname(__FILE__) + '/../../spec_helper'
require 'tempfile'

describe "Tempfile#path" do
  it "returns the path to the tempfile" do
    @tempfile = Tempfile.new("specs", tmp(""))
    @tempfile.path.should =~ /^#{tmp("")}/
    @tempfile.path.should include("specs")
  end
end
