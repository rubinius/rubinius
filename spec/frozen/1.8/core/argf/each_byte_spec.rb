require File.dirname(__FILE__) + '/../../spec_helper'

describe "ARGF.each_byte" do
  before :each do
    @file1_name = fixture __FILE__, "file1.txt"
    @file2_name = fixture __FILE__, "file2.txt"

    @bytes = []
    File.read(@file1_name).each_byte { |b| @bytes << b }
    File.read(@file2_name).each_byte { |b| @bytes << b }
  end

  after :each do
    ARGF.close
  end

  it "reads each byte of files" do
    argv [@file1_name, @file2_name] do
      bytes = []
      ARGF.each_byte { |b| bytes << b }
      bytes.should == @bytes
    end
  end
end
