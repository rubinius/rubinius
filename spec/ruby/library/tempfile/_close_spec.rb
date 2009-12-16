require File.dirname(__FILE__) + '/../../spec_helper'
require 'tempfile'

describe "Tempfile#_close" do
  before(:each) do
    @tempfile = Tempfile.new("specs")
  end
  
  it "is protected" do
    @tempfile.protected_methods.should include("_close")
  end
  
  it "closes self" do
    @tempfile.send(:_close)
    @tempfile.closed?.should be_true
  end
end
