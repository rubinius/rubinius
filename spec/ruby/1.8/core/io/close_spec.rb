require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#close" do
  before :each do
    @io = IO.popen(RUBY_NAME, "r+")
  end
  after :each do
    @io.close unless @io.closed?
  end  
  
  it "closes the stream" do
    lambda { @io.close }.should_not raise_error
    @io.closed?.should == true
  end

  it "returns nil" do
    @io.close.should == nil
  end
  
  it "makes the stream unavailable for any further data operations" do
    @io.close
    lambda { @io.print "attempt to write" }.should raise_error(IOError)
    lambda { @io.syswrite "attempt to write" }.should raise_error(IOError)
    lambda { @io.read }.should raise_error(IOError)
    lambda { @io.sysread(1) }.should raise_error(IOError)
  end

  it "raises an IOError on subsequent invocations" do
    @io.close
    lambda { @io.close }.should raise_error(IOError)
    lambda { @io.close }.should raise_error(IOError)
  end

  it "sets $? if the stream is opened by IO.popen" do
    @io.close
    $?.should == 0

    old_stderr = $stderr.dup
    $stderr.reopen("/dev/null")
    begin
      @io = IO.popen("does-not-exist-for-sure", "r+")
      @io.close
      $?.exitstatus.should == 127
    ensure
      $stderr.reopen(old_stderr)
    end
  end
end
