require File.dirname(__FILE__) + '/../../spec_helper'

describe "ARGF.skip" do
  before :each do
    @file1_name = fixture __FILE__, "file1.txt"
    @file2_name = fixture __FILE__, "file2.txt"

    @file2 = File.readlines @file2_name
  end

  after :each do
    ARGF.close
  end

  it "skips the current file" do
    argv [@file1_name, @file2_name] do
      ARGF.read(1)
      ARGF.skip
      ARGF.gets.should == @file2.first
    end
  end

  it "has no effect when called twice in a row" do
    argv [@file1_name, @file2_name] do
      ARGF.read(1)
      ARGF.skip
      ARGF.skip
      ARGF.gets.should == @file2.first
    end
  end

  it "has no effect at end of stream" do
    argv [@file1_name, @file2_name] do
      ARGF.read
      ARGF.skip
      ARGF.gets.should == nil
    end
  end
end
