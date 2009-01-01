require File.dirname(__FILE__) + '/../../spec_helper'

describe "ARGF.lineno" do
  before :each do
    @file1 = fixture __FILE__, "file1.txt"
    @file2 = fixture __FILE__, "file2.txt"
  end

  after :each do
    ARGF.close
  end

  # NOTE: this test assumes that fixtures files have two lines each
  # TODO: break this into four specs
  it "returns the current line number on each file" do
    argv [@file1, @file2, @file1, @file2] do
      ARGF.gets
      ARGF.lineno.should == 1
      ARGF.gets
      ARGF.lineno.should == 2
      ARGF.gets
      ARGF.lineno.should == 3
      ARGF.gets
      ARGF.lineno.should == 4

      ARGF.rewind
      ARGF.lineno.should == 4
      ARGF.gets
      ARGF.lineno.should == 3

      ARGF.lineno = 1000
      $..should == 1000
      ARGF.gets
      $..should == 1001
      ARGF.gets
      $..should == 1002

      $. = 2000
      ARGF.gets
      $..should == 2001
      ARGF.gets
      $..should == 2002
      ARGF.read
      $..should == 2002
    end
  end
end
