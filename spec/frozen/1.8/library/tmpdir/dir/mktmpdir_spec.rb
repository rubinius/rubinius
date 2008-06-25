require File.dirname(__FILE__) + "/../../../spec_helper"
require "tmpdir"

ruby_version_is "1.8.7" do
  describe "Dir.mktmpdir when passed no arguments" do
    it "returns the path to the created tmp-dir" do
      Dir.stub!(:mkdir)
      Dir.should_receive(:tmpdir).and_return("/tmp")
      path = Dir.mktmpdir
      path.should =~ /^\/tmp\//
    end
  
    it "creates a new writable directory in the path provided by Dir.tmpdir" do
      Dir.should_receive(:tmpdir).and_return(tmp(""))
      path = Dir.mktmpdir
      File.directory?(path).should be_true
      File.writable?(path).should be_true
    end
  end

  describe "Dir.mkdir when passed a block" do
    before(:each) do
      Dir.stub!(:tmpdir).and_return("/tmp")
      FileUtils.stub!(:remove_entry_secure)
    end
  
    it "yields the path to the passed block" do
      Dir.stub!(:mkdir)
      called = nil
      Dir.mktmpdir do |path|
        called = true
        path.should =~ /^\/tmp\//
      end
      called.should be_true
    end
  
    it "creates the tmp-dir before yielding" do
      Dir.should_receive(:tmpdir).and_return(tmp(""))
      Dir.mktmpdir do |path|
        File.directory?(path).should be_true
        File.writable?(path).should be_true
      end
    end
  
    it "removes the tmp-dir after executing the block" do
      Dir.stub!(:mkdir)
      Dir.mktmpdir do |path|
        FileUtils.should_receive(:remove_entry_secure).with(path)
      end
    end
  
    it "returns the blocks return value" do
      Dir.mktmpdir { :test }.should equal(:test)
    end
  end

  describe "Dir.mktmpdir when passed [String]" do
    before(:each) do
      Dir.stub!(:mkdir)
      Dir.stub!(:tmpdir).and_return("/tmp")
    end

    it "uses the passed String as a prefix to the tmp-directory" do
      prefix = "before"
      path = Dir.mktmpdir(prefix)
      path.should =~ /^\/tmp\/#{prefix}/
    end
  end

  describe "Dir.mktmpdir when passed [Array]" do
    before(:each) do
      Dir.stub!(:mkdir)
      Dir.stub!(:tmpdir).and_return("/tmp")
      FileUtils.stub!(:remove_entry_secure)
    end

    it "uses the first element of the passed Array as a prefix and the scond element as a suffix to the tmp-directory" do
      prefix = "before"
      suffix = "after"
    
      path = Dir.mktmpdir([prefix, suffix])
      path.should =~ /#{suffix}$/
    end
  end

  describe "Dir.mktmpdir when passed [Object]" do
    it "raises an ArgumentError" do
      lambda { Dir.mktmpdir(Object.new) }.should raise_error(ArgumentError)
      lambda { Dir.mktmpdir(:symbol) }.should raise_error(ArgumentError)
      lambda { Dir.mktmpdir(10) }.should raise_error(ArgumentError)
    end
  end
end