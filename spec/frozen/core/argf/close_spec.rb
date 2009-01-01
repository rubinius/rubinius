require File.dirname(__FILE__) + '/../../spec_helper'

describe "ARGF.close" do
  before :each do
    @file1_name = fixture __FILE__, "file1.txt"
    @file2_name = fixture __FILE__, "file2.txt"
  end

  after :each do
    ARGF.close
  end

  it "closes the current open stream" do
    argv [@file1_name, @file2_name] do
      io = ARGF.to_io
      ARGF.close
      io.closed?.should be_true
    end
  end
  
  it "returns self" do
    argv [@file1_name, @file2_name] do
      ARGF.close.should equal(ARGF)
    end
  end
end
