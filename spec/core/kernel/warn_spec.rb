require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.warn" do
  it "should call #write on $stderr" do
    begin
      v = $VERBOSE
      s = $stderr
      $VERBOSE = true
      $stderr = dev_null
      warn("Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn")
      $stderr.data.should == "Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn\n"
    ensure
      $VERBOSE = v
      $stderr = s
    end
  end

  it "should write the default record seperator (\\n) and NOT $/ to $stderr after the warning message" do
    begin
      v = $VERBOSE
      s = $stderr
      rs = $/
      $VERBOSE = true
      $/ = 'rs'
      $stderr = dev_null
      warn("")
      $stderr.data.should == "\n"
    ensure
      $VERBOSE = v
      $stderr = s
      $/ = rs
    end
  end

  it "should not call #write on $stderr if $VERBOSE is nil" do
    begin
      v = $VERBOSE
      s = $stderr
      $VERBOSE = nil
      $stderr = dev_null
      warn("Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn")
      $stderr.data.should == ""
    ensure
      $stderr = s
      $VERBOSE = v
    end
  end
end
