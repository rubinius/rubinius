require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/getc'

describe "ARGF.getc" do
  it_behaves_like :argf_getc, :getc
end

describe "ARGF.getc" do
  before :each do
    @file1 = fixture __FILE__, "file1.txt"
    @file2 = fixture __FILE__, "file2.txt"
  end

  after :each do
    ARGF.close unless ARGF.closed?
  end

  it "returns nil when end of stream reached" do
    argv [@file1, @file2] do
      ARGF.read
      ARGF.getc.should == nil
    end
  end
end
