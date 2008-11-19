require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "ARGF.readline" do
  
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

  it "reads one line of a file" do
    ARGFSpecs.file_args('file1.txt')
    ARGF.readline().should == @contents_file1.split($/).first+$/
  end

  it "reads all lines of a file" do
    ARGFSpecs.file_args('file1.txt')
    number_of_lines = @contents_file1.split($/).size
    all_lines = []
    for i in 1..number_of_lines
      all_lines << ARGF.readline()
    end
    all_lines.should == @contents_file1.split($/).collect { |l| l+$/ }
  end

  it "reads all lines of stdin" do
    ARGFSpecs.file_args('-')
    number_of_lines = @contents_stdin.split($/).size
    all_lines = []
    STDIN.reopen(File.dirname(__FILE__) + '/fixtures/stdin.txt')
    for i in 1..number_of_lines
      all_lines << ARGF.readline()
    end
    all_lines.should == @contents_stdin.split($/).collect { |l| l+$/ }
  end

  it "reads all lines of two files" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    number_of_lines = @contents_file1.split($/).size + @contents_file2.split($/).size
    all_lines = []
    for i in 1..number_of_lines
      all_lines << ARGF.readline()
    end
    all_lines.should == (@contents_file1+ @contents_file2).split($/).collect { |l| l+$/ }
  end

  it "raises an EOFError when reaching end of files" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    lambda { while line = ARGF.readline;end }.should raise_error(EOFError)
  end

  # Note: this test will only work on platforms that is capable of doing
  # safe rename. Unfortunately there is no method in 1.8.X to test that.
  # This has been corrected in Ruby 1.9.x
  ruby_version_is "1.9" do
    it "modifies the files when in place edit mode is on" do

      ARGFSpecs.ruby(:options =>['-i'], :code => <<-SRC, :args => [@file1, @file2]) do |f|
        begin
          while line = ARGF.readline
            puts line.chomp+'.new'
          end
        rescue EOFError
        end
      SRC
        File.read(@file1).should == @contents_file1.split($/).collect { |l| l+'.new'+$/}.join
        File.read(@file2).should == @contents_file2.split($/).collect { |l| l+'.new'+$/}.join
      end

    end

    it "modifies and backups two files when in place edit mode is on" do

      ARGFSpecs.ruby(:options =>['-i.bak'], :code => <<-SRC, :args => [@file1, @file2]) do |f|
          begin
            while line = ARGF.readline
              puts line.chomp+'.new'
            end
          rescue EOFError
          end
        SRC
        File.read(@file1).should == @contents_file1.split($/).collect { |l| l+'.new'+$/}.join
        File.read(@file2).should == @contents_file2.split($/).collect { |l| l+'.new'+$/}.join
        File.read(@file1+'.bak').should == @contents_file1
        File.read(@file2+'.bak').should == @contents_file2
      end
    end
  end
end