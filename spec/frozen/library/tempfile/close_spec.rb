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
    # Explicitly close the tempfile, as we
    # might stub/mock File.unlink calls.
    @tempfile.close
  end
  
  it "closes self" do
    @tempfile.close(true)
    @tempfile.closed?.should be_true
  end
  
  it "unlinks self" do
    File.should_receive(:unlink)
    @tempfile.close(true)
  end
end

describe "Tempfile#close!" do
  before(:each) do
    @tempfile = Tempfile.new("specs", tmp(""))
  end
  
  after(:each) do
    # Explicitly close the tempfile, as we
    # might stub/mock File.unlink calls.
    @tempfile.close
  end
  
  it "closes self" do
    @tempfile.close!
    @tempfile.closed?.should be_true
  end
  
  it "unlinks self" do
    File.should_receive(:unlink)
    @tempfile.close!
  end
end
