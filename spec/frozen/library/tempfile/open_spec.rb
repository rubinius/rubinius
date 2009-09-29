require File.dirname(__FILE__) + '/../../spec_helper'
require 'tempfile'

describe "Tempfile#open" do
  before(:each) do
    @tempfile = Tempfile.new("specs")
    @tempfile.puts("Test!")
  end
  
  after(:each) do
    @tempfile.close
  end

  it "reopens self" do
    @tempfile.close
    @tempfile.open
    @tempfile.closed?.should be_false
  end
  
  it "reopens self in read and write mode and does not truncate" do
    @tempfile.open
    @tempfile.puts("Another Test!")
    
    @tempfile.open
    @tempfile.readline.should == "Another Test!\n"
  end
end

describe "Tempfile.open" do
  it "returns a new, open Tempfile instance" do
    # Tempfile.open("specs").should be_kind_of(Tempfile) # => fails!
    t = Tempfile.open("specs")
    t.instance_of?(Tempfile).should be_true
    t.close
  end
end

describe "Tempfile.open when passed a block" do
  it "yields a new, open Tempfile instance to the block" do
    yielded = false
    Tempfile.open("specs") do |tmpfile|
      yielded = true
      #tmpfile.should be_kind_of(Tempfile)
      tmpfile.instance_of?(Tempfile).should be_true
      tmpfile.closed?.should be_false
    end
    yielded.should be_true
  end
  
  it "closes the yielded Tempfile after the block" do
    tempfile = nil
    Tempfile.open("specs") { |x| tempfile = x }
    tempfile.closed?.should be_true
  end
end

ruby_version_is '1.8.7' .. '1.9' do
  describe "Tempfile.open" do
    it "is passed an array [base,suffix] as first argument" do
      tempfile = nil
      Tempfile.open(["specs", ".tt"]) { |x| tempfile = x }
      tempfile.path.should =~ /specs.*\.tt$/
    end
  end
end
