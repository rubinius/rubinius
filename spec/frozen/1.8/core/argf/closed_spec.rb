require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "ARGF.closed?" do
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
  it "says it is closed " do
    ARGFSpecs.file_args('file1.txt', 'file2.txt', '-')
    STDIN.reopen(File.dirname(__FILE__) + '/fixtures/stdin.txt')
    res = []
    3.times do
      res << ARGF.closed?
      ARGF.gets
      ARGF.gets
    end
    res << ARGF.closed?
    ARGF.gets
    res << ARGF.closed?

    res.should == [false, false, false, false, true]
  end
  
end