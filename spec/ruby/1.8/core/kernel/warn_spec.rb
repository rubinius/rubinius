require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.warn" do
  it "calls #write on $stderr" do
    lambda {
      v = $VERBOSE
      $VERBOSE = true

      warn("Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn")

      $VERBOSE = v
    }.should output(nil, "Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn\n")
  end

  it "writes the default record seperator (\\n) and NOT $/ to $stderr after the warning message" do
    lambda {
      v = $VERBOSE
      rs = $/
      $VERBOSE = true
      $/ = 'rs'

      warn("")

      $VERBOSE = v
      $/ = rs
    }.should output(nil, "\n")
  end

  it "does not call #write on $stderr if $VERBOSE is nil" do
    lambda {
      v = $VERBOSE
      $VERBOSE = nil

      warn("Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn")

      $VERBOSE = v
    }.should output(nil, "")
  end
end
