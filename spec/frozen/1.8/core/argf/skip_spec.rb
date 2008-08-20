require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "ARGF.skip" do
    before :each do
    ARGV.clear
    @file1 = ARGFSpecs.fixture_file('file1.txt')
    @file2 = ARGFSpecs.fixture_file('file2.txt')
    @stdin = ARGFSpecs.fixture_file('stdin.txt')
    @contents_file1 = File.read(@file1)
    @contents_file2 = File.read(@file2)
    @contents_stdin = File.read(@stdin)
    @linef1_1, @linef1_2 =  @contents_file1.split($/).collect { |l| l+$/ }
    @linef2_1, @linef2_2 =  @contents_file2.split($/).collect { |l| l+$/ }
  end

  after :each do
    # Close any open file (catch exception if already closed)
    ARGF.close rescue nil
    ARGFSpecs.fixture_file_delete(@file1,@file2,@stdin)
  end

  it "skips the current file" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    stg = ""
    ARGF.read(1)
    ARGF.skip; stg << ARGF.gets
    stg.should == @linef2_1
  end
  
  it "has no effect when called twice in a row" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    stg = ""
    ARGF.read(1)
    ARGF.skip
    ARGF.skip; stg << ARGF.gets
    stg.should == @linef2_1
  end
  
    it "has no effect at end of stream" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    ARGF.read
    ARGF.skip
    ARGF.gets.should == nil
  end
  
  
end