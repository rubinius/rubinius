describe :argf_eof, :shared => true do
  
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
    ARGF.close
    ARGFSpecs.fixture_file_delete(@file1,@file2,@stdin)
  end

  # NOTE: this test assumes that fixtures files have two lines each
  # SO DO NOT modify the fixture files!!!
  it "returns true when reaching the end of a file" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt', 'file1.txt')
    res = []
    # should say false, true alternatively with 2 line files  
    while ARGF.gets
      res << ARGF.send(@method)
    end
    res.should == [false, true, false, true, false, true]
  end

  it "raises IOError when called on a closed stream" do
    ARGFSpecs.file_args('file1.txt')
    ARGF.read # read everything at once. Stream closed.
    lambda { ARGF.send(@method) }.should raise_error(IOError)
  end
end
