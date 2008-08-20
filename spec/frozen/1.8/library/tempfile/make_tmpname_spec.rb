require File.dirname(__FILE__) + '/../../spec_helper'
require 'tempfile'

describe "Tempfile#make_tmpname" do
  before(:each) do
    @tempfile = Tempfile.new("specs")
  end
  
  it "is private" do
    Tempfile.private_instance_methods.should include("make_tmpname")
  end
end

describe "Tempfile#make_tmpname when passed [String, num]" do
  before(:each) do
    @tempfile = Tempfile.new("specs")
  end

  it "uses the passed String as a prefix to the filename" do
    @tempfile.send(:make_tmpname, "test", 0).should =~ /^test/
    @tempfile.send(:make_tmpname, "spec", 0).should =~ /^spec/
  end
  
  it "includes the passed num in the filename" do
    @tempfile.send(:make_tmpname, "test", 10).should include("10")
    @tempfile.send(:make_tmpname, "test", 20).should include("20")
  end
end

ruby_version_is "1.8.7" do
  describe "Tempfile#make_tmpname when passed [Array, num]" do
    before(:each) do
      @tempfile = Tempfile.new("specs")
    end

    it "uses the first element of the passed Array as a prefix to the filename" do
      @tempfile.send(:make_tmpname, ["prefix", "suffix"], 0).should =~ /^prefix/
      @tempfile.send(:make_tmpname, ["one", "two"], 0).should =~ /^one/
    end
  
    it "uses the second element of the passed Array as a suffix to the filename" do
      @tempfile.send(:make_tmpname, ["prefix", "suffix"], 0).should =~ /suffix$/
      @tempfile.send(:make_tmpname, ["one", "two"], 0).should =~ /two$/
    end

    it "includes the passed num in the filename" do
      @tempfile.send(:make_tmpname, ["prefix", "suffix"], 10).should include("10")
      @tempfile.send(:make_tmpname, ["one", "two"], 20).should include("20")
    end
  end
end
