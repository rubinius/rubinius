describe :argf_each_line, :shared => true do
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

  it "reads each line of files" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt', '-')
    STDIN.reopen(@stdin)
    line_list = []
    ARGF.send(@method) { |b| line_list << b }
    line_list.should == (@contents_file1 + @contents_file2 + @contents_stdin).split($/).collect { |l| l+$/} # ord
  end
end
