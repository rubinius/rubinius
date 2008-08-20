require File.dirname(__FILE__) + '/fixtures/classes'

describe "ARGF.rewind" do
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
  it "goes back to beginning of current file" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    linef1_1, linef1_2 =  @contents_file1.split($/).collect { |l| l+$/ }
    linef2_1, linef2_2 =  @contents_file2.split($/).collect { |l| l+$/ }

    res =[]
    ARGF.gets;
    ARGF.rewind; res << ARGF.gets
    ARGF.gets # finish reading file1
    ARGF.gets
    ARGF.rewind; res << ARGF.gets

    res.should == [linef1_1, linef2_1]
    
  end
  
  it "raises ArgumentError when end of stream reached" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    ARGF.read 
    lambda { ARGF.rewind }.should raise_error(ArgumentError)
  end

end