require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/helpers/tmp'

describe Object, "#tmp" do
  before :each do
    File.stub!(:directory?).and_return(false)
    File.stub!(:symlink?).and_return(false)
    ENV.stub!(:[]).and_return(nil)
  end

  it "returns /tmp/<name> if /tmp is a writable directory" do
    dir = "/tmp"
    File.should_receive(:directory?).with(dir).and_return(true)
    File.should_receive(:writable?).with(dir).and_return(true)
    File.should_receive(:expand_path).with(dir).and_return(dir)
    tmp("test.txt").should == dir + "/test.txt"
  end

  it "returns /var/tmp/<name> if /var/tmp is a writable directory" do
    dir = "/var/tmp"
    File.should_receive(:directory?).with(dir).and_return(true)
    File.should_receive(:writable?).with(dir).and_return(true)
    File.should_receive(:expand_path).with(dir).and_return(dir)
    tmp("test.txt").should == dir + "/test.txt"
  end

  it "returns ENV['TMPDIR']/<name> if ENV['TMPDIR'] is a writable directory" do
    dir = "/tmpdir"
    ENV.should_receive(:[]).with("TMPDIR").and_return(dir)
    File.should_receive(:directory?).with(dir).and_return(true)
    File.should_receive(:writable?).with(dir).and_return(true)
    File.should_receive(:expand_path).with(dir).and_return(dir)
    tmp("test.txt").should == dir + "/test.txt"
  end

  it "returns ENV['TMP']/<name> if ENV['TMP'] is a writable directory" do
    dir = "/tmp/tmp"
    ENV.should_receive(:[]).with("TMP").and_return(dir)
    File.should_receive(:directory?).with(dir).and_return(true)
    File.should_receive(:writable?).with(dir).and_return(true)
    File.should_receive(:expand_path).with(dir).and_return(dir)
    tmp("test.txt").should == dir + "/test.txt"
  end

  it "returns ENV['TEMP']/<name> if ENV['TEMP'] is a writable directory" do
    dir = "/tmp/temp"
    ENV.should_receive(:[]).with("TEMP").and_return(dir)
    File.should_receive(:directory?).with(dir).and_return(true)
    File.should_receive(:writable?).with(dir).and_return(true)
    File.should_receive(:expand_path).with(dir).and_return(dir)
    tmp("test.txt").should == dir + "/test.txt"
  end

  it "returns ENV['USERPROFILE']/<name> if ENV['USERPROFILE'] is a writable directory" do
    dir = "/tmp/temp"
    ENV.should_receive(:[]).with("TEMP").and_return(dir)
    File.should_receive(:directory?).with(dir).and_return(true)
    File.should_receive(:writable?).with(dir).and_return(true)
    File.should_receive(:expand_path).with(dir).and_return(dir)
    tmp("test.txt").should == dir + "/test.txt"
  end

  it "returns the actual file name if the file is a symlink" do
    dir = "/tmp"
    File.should_receive(:directory?).with(dir).and_return(true)
    File.should_receive(:writable?).with(dir).and_return(true)
    File.should_receive(:expand_path).with(dir).and_return(dir)
    File.should_receive(:symlink?).with(dir).and_return(true)
    File.should_receive(:readlink).with(dir).and_return("/ponies"+dir)
    tmp("test.txt").should == "/ponies" + dir + "/test.txt"
  end
end
