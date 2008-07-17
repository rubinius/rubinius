require File.dirname(__FILE__) + '/fixtures/classes'

describe "ARGF.lineno" do
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
  it "returns the current line number on each file" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt', 'file1.txt', 'file2.txt')
    res = []
    
    ARGF.gets; res << ARGF.lineno # 1
    ARGF.gets; res << ARGF.lineno # 2
    ARGF.gets; res << ARGF.lineno # 3
    ARGF.gets; res << ARGF.lineno # 4
    ARGF.rewind; res << ARGF.lineno # 4
    ARGF.gets; res << ARGF.lineno # 3
    ARGF.lineno = 1000; res << $.
    ARGF.gets; res << $. # 1001
    ARGF.gets; res << $. # 1002
    $. = 2000
    ARGF.gets; res << $. # 2001
    ARGF.gets; res << $. # 2002
    ARGF.read; res << $. # 2002 (beyond end of stream)

    res.should == [1, 2, 3, 4, 4, 3, 1000, 1001, 1002, 2001, 2002, 2002]
  end

end