require File.dirname(__FILE__) + '/../../spec_helper'

describe "ARGF.close" do
  before :each do
    @file1_name = fixture __FILE__, "file1.txt"
    @file2_name = fixture __FILE__, "file2.txt"

    @file1 = File.readlines @file1_name
    @file2 = File.read @file2_name
  end

  after :each do
    ARGF.close
  end

  it "closes the current file and read the next one" do
    argv [@file1_name, @file2_name] do
      ARGF.close
      ARGF.read.should == @file2
    end
  end

  it "reads one line from the first file, closes it and read the next one" do
    argv [@file1_name, @file2_name] do
      ARGF.gets.should == @file1.first
      ARGF.close
      ARGF.read.should == @file2
    end
  end
end
