describe :argf_filename, :shared => true do
  
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
  it "returns the current file name on each file" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')

    res = []
    # returns first current file even when not yet open
    begin
      res << ARGF.send(@method)
    end while ARGF.gets
    # returns last current file even when closed
    res << ARGF.send(@method)
    res.collect { |f| File.expand_path(f) }.should == [@file1, @file1, @file1, @file2, @file2, @file2]
  end

  # NOTE: this test assumes that fixtures files have two lines each
  # SO DO NOT modify the fixture files!!!
  it "it sets the $FILENAME global variable with the current file name on each file" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    res = []
    while ARGF.gets
      res << $FILENAME
    end 
    # returns last current file even when closed
    res << $FILENAME
    res.collect { |f| File.expand_path(f) }.should == [@file1, @file1, @file2, @file2, @file2]
  end
end
