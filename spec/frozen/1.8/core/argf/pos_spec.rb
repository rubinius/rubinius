require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/pos'

describe "ARGF.pos" do
  it_behaves_like(:argf_pos, :pos)
end

describe "ARGF.pos=" do
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
  it "sets the correct position in files" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    linef1_1, linef1_2 =  @contents_file1.split($/).collect { |l| l+$/ }
    linef2_1, linef2_2 =  @contents_file2.split($/).collect { |l| l+$/ }

    res =[]
    ARGF.pos = linef1_1.size; res << ARGF.gets
    ARGF.pos = 0; res << ARGF.gets
    ARGF.gets # finish reading file1
    ARGF.gets
    ARGF.pos = 1; res << ARGF.gets
    ARGF.pos = linef2_1.size + linef2_2.size - 1; res << ARGF.gets
    ARGF.pos = 1000; res << ARGF.read

    res.should == [linef1_2, linef1_1, linef2_1[1..-1], linef2_2[-1,1], ""]
    
  end
  

end