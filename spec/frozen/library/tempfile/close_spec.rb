require File.dirname(__FILE__) + '/../../spec_helper'
require 'tempfile'

describe "Tempfile#close when passed no argument or [false]" do
  before(:each) do
    @tempfile = Tempfile.new("specs", tmp(""))
  end
  
  it "closes self" do
    @tempfile.close
    @tempfile.closed?.should be_true
  end
end

describe "Tempfile#close when passed [true]" do
  before(:each) do
    @tempfile = Tempfile.new("specs", tmp(""))
  end
  
  after(:each) do
    @tempfile.unlink
  end
  
  it "closes self" do
    @tempfile.close(true)
    @tempfile.closed?.should be_true
  end
  
  it "unlinks self" do
    @tempfile.close(true)
    File.exists?(@tempfile.path).should be_false
  end
end

describe "Tempfile#close!" do
  before(:each) do
    @tempfile = Tempfile.new("specs", tmp(""))
  end
  
  after(:each) do
    @tempfile.unlink
  end
  
  it "closes self" do
    @tempfile.close!
    @tempfile.closed?.should be_true
  end
  
  it "unlinks self" do
    @tempfile.close!
    File.exists?(@tempfile.path).should be_false
  end
end
