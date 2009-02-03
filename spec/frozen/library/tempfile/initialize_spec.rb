require File.dirname(__FILE__) + '/../../spec_helper'
require 'tempfile'

describe "Tempfile#initialize" do
  before(:each) do
    @tempfile = Tempfile.allocate
  end

  after(:each) do
    @tempfile.close
  end

  it "opens a new tempfile with the passed name in the passed directory" do
    @tempfile.send(:initialize, "basename", tmp(""))
    File.exist?(@tempfile.path).should be_true
    @tempfile.path.should =~ /^#{tmp("")}/
    @tempfile.path.should include("basename")
  end

  it "sets the permisssions on the tempfile to 0600" do
    @tempfile.send(:initialize, "basename", tmp(""))
    File.stat(@tempfile.path).mode.should == 0100600
  end
end
