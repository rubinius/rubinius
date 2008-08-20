require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "ARGF.close" do
  before :each do
    ARGV.clear
    @file1 = ARGFSpecs.fixture_file('file1.txt')
    @file2 = ARGFSpecs.fixture_file('file2.txt')
    @stdin = ARGFSpecs.fixture_file('stdin.txt')
    @contents_file1 = File.read(@file1)
    @contents_file2 = File.read(@file2)
    @contents_stdin = File.read(@stdin)
  end

  after :each do
    # Close any open file (catch exception if already closed)
    ARGF.close rescue nil
    ARGFSpecs.fixture_file_delete(@file1,@file2,@stdin)
  end
  
  it "closes the current file and read the next one" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    ARGF.close
    ARGF.read.should == @contents_file2
  end
  
  it "reads one line from the first file, closes it and read the next one" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    ARGF.gets.should == @contents_file1.split($/).first+$/
    ARGF.close
    ARGF.read.should == @contents_file2
  end
  
end