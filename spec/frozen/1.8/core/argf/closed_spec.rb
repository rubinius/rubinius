require File.dirname(__FILE__) + '/../../spec_helper'

describe "ARGF.closed?" do
  before :each do
    @file1_name = fixture __FILE__, "file1.txt"
    @file2_name = fixture __FILE__, "file2.txt"

    @file1 = File.readlines @file1_name
    @file2 = File.read @file2_name
  end

  after :each do
    ARGF.close
  end

  # NOTE: this test assumes that fixtures files have two lines each
  it "returns true if the file is closed" do
    argv [@file1_name, @file2_name, @file1_name] do
      3.times do
        ARGF.closed?.should be_false
        ARGF.gets
        ARGF.gets
      end
      ARGF.closed?.should be_false
      ARGF.gets
      ARGF.closed?.should be_true
    end
  end
end
