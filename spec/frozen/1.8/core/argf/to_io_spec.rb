require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "ARGF.to_io" do
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
  it "returns the IO of the current file" do
    ARGFSpecs.file_args('file1.txt', '-', 'file2.txt')
    STDIN.reopen(@stdin)
    res = []
    ARGF.gets; res << ARGF.to_io
    ARGF.gets; res << ARGF.to_io
    ARGF.gets; res << ARGF.to_io
    ARGF.gets; res << ARGF.to_io
    ARGF.gets; res << ARGF.to_io
    ARGF.gets; res << ARGF.to_io
    res.collect { |io| io.kind_of? IO }.should == [true] * 6
    res[0].should == res[1]
    res[2].should == res[3]
    res[4].should == res[5]
  end

end