require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "ARGF.file" do
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
  
  # NOTE: this test assumes that fixtures files have two lines each
  # SO DO NOT modify the fixture files!!!
  it "returns the current file object on each file" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    res = []
    # returns first current file even when not yet open
    begin
      res << ARGF.file.path
    end while ARGF.gets
    # returns last current file even when closed
    res << ARGF.file.path
    res.should == %w{file1 file1 file1 file2 file2 file2}.collect { |bn| tmp(bn+'.txt')}
  end

end