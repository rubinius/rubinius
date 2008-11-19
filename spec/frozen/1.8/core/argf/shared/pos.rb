describe :argf_pos, :shared => true do

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

  it "gives the correct position for each read operation" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    res = []
    N1 = @contents_file1.size
    N2 = @contents_file2.size
    ARGF.read(2); res << ARGF.send(@method)
    ARGF.read(N1-2); res << ARGF.send(@method)
    ARGF.read(6); res << ARGF.send(@method)
    ARGF.rewind; res << ARGF.send(@method)
    ARGF.read(N2); res << ARGF.send(@method)
  
    res.should == [2, N1, 6, 0, N2]
  
  end

  it "raises ArgumentError (no stream) when called on a closed stream" do
    ARGFSpecs.file_args('file1.txt')
    ARGF.read # read everything at once. Stream closed.
    lambda { ARGF.send(@method) }.should raise_error(ArgumentError)
  end
end
