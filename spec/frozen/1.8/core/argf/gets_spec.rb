require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "ARGF.gets" do
  
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
    ARGF.gets().should == @contents_file1.split($/).first+$/
  end

  it "reads all lines of a file" do
    ARGFSpecs.file_args('file1.txt')
    number_of_lines = @contents_file1.split($/).size
    all_lines = []
    for i in 1..number_of_lines
      all_lines << ARGF.gets()
    end
    all_lines.should == @contents_file1.split($/).collect { |l| l+$/ }
  end

  it "reads all lines of stdin" do
    ARGFSpecs.ruby(:options => [], :code => <<-SRC, :args => ['-', "< #{@stdin}"]) do |f|
        while line = ARGF.gets
          print line
        end
      SRC
      f.read.should == @contents_stdin
    end
  end

  it "reads all lines of two files" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    number_of_lines = @contents_file1.split($/).size + @contents_file2.split($/).size
    all_lines = []
    for i in 1..number_of_lines
      all_lines << ARGF.gets()
    end
    all_lines.should == (@contents_file1+ @contents_file2).split($/).collect { |l| l+$/ }
  end

  it "returns nil when reaching end of files" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    number_of_lines = @contents_file1.split($/).size + @contents_file2.split($/).size
    for i in 1..number_of_lines
      ARGF.gets()
    end
    ARGF.gets.should == nil
  end

  it "sets $_ global variable with each line read" do
    ARGFSpecs.file_args('file1.txt', 'file2.txt')
    while line = ARGF.gets
      $_.should == line
    end
  end


#  # TODO: reactivate this code when in place edit mode works ok in argf.rb
#  # Note: this test will only work on platforms that is capable of doing
#  # safe rename. Unfortunately there is no method in 1.8.X to test that.
#  # This has been corrected in Ruby 1.9.x
#  it "modifies the files when in place edit mode is on" do
#
#    ARGFSpecs.ruby(:options =>['-i'], :code => <<-SRC, :args => [@file1, @file2]) do |f|
#          while line = ARGF.gets
#            puts line.chomp+'.new'
#          end
#      SRC
#      File.read(@file1).should == @contents_file1.split($/).collect { |l| l+'.new'+$/}.join
#      File.read(@file2).should == @contents_file2.split($/).collect { |l| l+'.new'+$/}.join
#    end
#
#  end
#    
#  # TODO: reactivate this code when in place edit mode works ok in argf.rb
#  it "modifies and backups two files when in place edit mode is on" do
#
#    ARGFSpecs.ruby(:options =>['-i.bak'], :code => <<-SRC, :args => [@file1, @file2]) do |f|
#          while line = ARGF.gets
#            puts line.chomp+'.new'
#          end
#      SRC
#      File.read(@file1).should == @contents_file1.split($/).collect { |l| l+'.new'+$/}.join
#      File.read(@file2).should == @contents_file2.split($/).collect { |l| l+'.new'+$/}.join
#      File.read(@file1+'.bak').should == @contents_file1
#      File.read(@file2+'.bak').should == @contents_file2
#    end
#  end

end
