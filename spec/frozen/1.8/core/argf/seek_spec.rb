require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "ARGF.seek" do
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

  it "sets the correct absolute position relative to beginning of file" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    linef1_1, linef1_2 =  @contents_file1.split($/).collect { |l| l+$/ }
    linef2_1, linef2_2 =  @contents_file2.split($/).collect { |l| l+$/ }

    res =[]
    ARGF.seek(2); res << ARGF.gets
    ARGF.seek(linef1_1.size); res << ARGF.gets
    ARGF.seek(0, IO::SEEK_END); res << ARGF.gets

    res.should == [linef1_1[2..-1], linef1_2, linef2_1]
  end
  
  it "sets the correct position relative to current position in file" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    linef1_1, linef1_2 =  @contents_file1.split($/).collect { |l| l+$/ }
    linef2_1, linef2_2 =  @contents_file2.split($/).collect { |l| l+$/ }

    res =[]
    ARGF.seek(0, IO::SEEK_CUR); res << ARGF.gets
    ARGF.seek(-linef1_1.size+2, IO::SEEK_CUR); res << ARGF.gets
    ARGF.seek(1, IO::SEEK_CUR); res << ARGF.gets
    ARGF.seek(3, IO::SEEK_CUR); res << ARGF.gets # seek doesn't go beyond file limit
    ARGF.seek(linef1_2.size, IO::SEEK_CUR); res << ARGF.gets 

    res.should == [linef1_1, linef1_1[2..-1], linef1_2[1..-1], linef2_1, nil]
  end
  
  it "sets the correct absolute position relative to end of file" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    linef1_1, linef1_2 =  @contents_file1.split($/).collect { |l| l+$/ }
    linef2_1, linef2_2 =  @contents_file2.split($/).collect { |l| l+$/ }

    res =[]
    ARGF.seek(-linef1_1.size-linef1_2.size, IO::SEEK_END); res << ARGF.gets
    ARGF.seek(-6, IO::SEEK_END); res << ARGF.gets
    ARGF.seek(-4, IO::SEEK_END); res << ARGF.gets
    res << ARGF.gets
    ARGF.seek(-6, IO::SEEK_END); res << ARGF.gets

    res.should == [linef1_1, linef1_2[-6..-1], linef1_2[4..-1], linef2_1, linef2_2[-6..-1]]
  end
  
  it "takes at least one argument (offset)" do
    lambda { ARGF.seek() }.should raise_error(ArgumentError)
  end
  
end