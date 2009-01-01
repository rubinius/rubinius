require File.dirname(__FILE__) + '/../../spec_helper'

describe "ARGF.closed?" do
  before :each do
    @file1_name = fixture __FILE__, "file1.txt"
    @file2_name = fixture __FILE__, "file2.txt"
  end

  after :each do
    ARGF.close
  end

  it "returns true if the current stream has been closed" do
    argv [@file1_name, @file2_name] do
      stream = ARGF.to_io
      stream.close
      
      ARGF.closed?.should be_true
    end
  end
end
