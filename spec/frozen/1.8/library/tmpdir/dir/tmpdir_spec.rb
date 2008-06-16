require File.dirname(__FILE__) + "/../../../spec_helper"
require "tmpdir"

describe "Dir.tmpdir when $SAFE > 0" do
  it "expands the path with Dir's @@systmpdir" do
    # That is a hack, I know
    Thread.start do
      $SAFE = 1
      class Dir
        @@systmpdir = "test"
      end
      File.should_receive(:expand_path).with("test")
      Dir.tmpdir
    end.join
  end
end

describe "Dir.tmpdir when $SAFE = 0" do
  before(:each) do
    File.stub!(:expand_path)
  end
  
  it "tries to get a tmp-path from different sources" do
    ENV.should_receive(:[]).with("TMPDIR")
    ENV.should_receive(:[]).with("TMP")
    ENV.should_receive(:[]).with("TEMP")
    ENV.should_receive(:[]).with("USERPROFILE")
    Dir.tmpdir
  end
  
  # TODO: RDoc does not say that the path gets extended
  #       by the temp-dir.
  it "expands the path with the first value that is a directory and is writable" do
    ENV.should_receive(:[]).with("TMPDIR").and_return("no_dir")
    ENV.should_receive(:[]).with("TMP").and_return("no_writable_dir")
    ENV.should_receive(:[]).with("TEMP").and_return("writable_dir")
    ENV.should_receive(:[]).with("USERPROFILE")

    File.should_receive(:directory?).with("no_dir").and_return(false)
    File.should_receive(:directory?).with("no_writable_dir").and_return(true)
    File.should_receive(:directory?).with("writable_dir").and_return(true)

    File.should_receive(:writable?).with("no_writable_dir").and_return(false)
    File.should_receive(:writable?).with("writable_dir").and_return(true)

    File.should_receive(:expand_path).with("writable_dir")

    Dir.tmpdir
  end
  
  it "expands the path by '.' if absolutely no writable directory is found" do
    File.stub!(:directory?).and_return(false)
    File.should_receive(:expand_path).with('.')
    Dir.tmpdir
  end
end