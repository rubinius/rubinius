require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.warn" do
  class FakeErr
    def initialize; @written = ''; end
    def written; @written; end
    def write(warning); @written << warning; end;
   end

  it "should call #write on $stderr" do
    s = $stderr
    $stderr = FakeErr.new
    warn("Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn")
    $stderr.written.should == "Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn\n"
    $stderr = s
  end

  it "should write the default record seperator (\\n) and NOT $/ to $stderr after the warning message" do
    s = $stderr
    rs = $/
    $/ = 'rs'
    $stderr = FakeErr.new
    warn("")
    $stderr.written.should == "\n"
    $stderr = s
    $/ = rs
  end

  it "should not call #write on $stderr if $VERBOSE is nil" do
    v = $VERBOSE
    $VERBOSE = nil
    s = $stderr
    $stderr = FakeErr.new
    warn("Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn")
    $stderr.written.should == ""
    $stderr = s
    $VERBOSE = v
  end
end
