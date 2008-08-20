require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "ARGF.read" do
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
    # Close any open file
    ARGF.close unless ARGF.closed?
    ARGFSpecs.fixture_file_delete(@file1,@file2,@stdin)
  end
  
  it "reads the contents of a file" do
    ARGFSpecs.file_args('file1.txt')
    ARGF.read().should == @contents_file1
  end

  it "treats first nil argument as no length limit" do
    ARGFSpecs.file_args('file1.txt')
    ARGF.read(nil).should == @contents_file1
  end
  
  it "treats second nil argument as no output buffer" do
    ARGFSpecs.file_args('file1.txt')
    ARGF.read(nil, nil).should == @contents_file1
  end
  
  it "treats second argument as an output buffer" do
    ARGFSpecs.file_args('file1.txt')
    buffer = ""
    ARGF.read(nil, buffer)
    buffer.should == @contents_file1
  end
  
  it "reads a number of bytes from the first file" do
    ARGFSpecs.file_args('file1.txt')
    ARGF.read(5).should == @contents_file1[0,5]
  end
  
  it "reads from a single file consecutively" do
    ARGFSpecs.file_args('file1.txt')
    ARGF.read(1).should == @contents_file1[0,1]
    ARGF.read(2).should == @contents_file1[1,2]
    ARGF.read(3).should == @contents_file1[3,3]
  end
  
  it "reads the contents of two files" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    ARGF.read.should ==  @contents_file1 + @contents_file2
  end
  
  it "reads the contents of one file and some characters from the second" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    len = @contents_file1.size + @contents_file2.size/2
    ARGF.read(len).should ==  (@contents_file1 + @contents_file2)[0,len]
  end
  
  it "reads across two files consecutively" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    ARGF.read(@contents_file1.length-2 ).should == @contents_file1[0..-3]
    ARGF.read(2+5).should == @contents_file1[-2..-1] + @contents_file2[0,5]
  end
  
  it "reads the contents of stdin" do
    ARGFSpecs.file_args('-')
    STDIN.reopen(ARGFSpecs.fixture_file('stdin.txt'))
    ARGF.read.should ==  @contents_stdin
  end
  
  it "reads a number of bytes from stdin" do
    ARGFSpecs.file_args('-')
    STDIN.reopen(ARGFSpecs.fixture_file('stdin.txt'))
    ARGF.read(10).should ==  @contents_stdin[0,10]
  end
  
  it "reads a number of bytes from stdin" do
    ARGFSpecs.file_args('-')
    STDIN.reopen(ARGFSpecs.fixture_file('stdin.txt'))
    ARGF.read(10).should ==  @contents_stdin[0,10]
  end
  
  it "reads the contents of one file and stdin" do
    ARGFSpecs.file_args('file1.txt', '-')
    STDIN.reopen(ARGFSpecs.fixture_file('stdin.txt'))
    ARGF.read.should ==  @contents_file1 + @contents_stdin
  end
  
  it "reads the contents of the same file twice" do
    ARGFSpecs.file_args('file1.txt', 'file1.txt')
    ARGF.read.should ==  @contents_file1 + @contents_file1
  end
  
  not_supported_on :windows do  
    it "reads the contents of a special device file" do
      ARGFSpecs.file_args('/dev/zero')
      ARGF.read(100).should ==  "\000" * 100
    end
  end
  
end