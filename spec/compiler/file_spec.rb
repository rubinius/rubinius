require File.dirname(__FILE__) + "/spec_helper"

describe Compiler::CompiledFile do
  it "loads in a CompiledFile from a stream" do
    str = "!RBIX\n1\n8a883234\nt"
    io = StringIO.new(str)

    cm = Compiler::CompiledFile.load(io)
    cm.should be_kind_of(Compiler::CompiledFile)
    cm.magic.should == "!RBIX"
    cm.version.should == 1
    cm.sum.should == "8a883234"
    io.tell.should == 17
  end

  it "encodes a to a stream" do
    cm = Compiler::CompiledFile.new("!RBIX", 10, "8", nil)
    stream = StringIO.new

    cm.encode_to stream, nil

    stream.string.should == "!RBIX\n10\n8\nn\n"
  end

  it "unmarshals the body" do
    str = "!RBIX\n1\n8a883234\nt"
    io = StringIO.new(str)

    cm = Compiler::CompiledFile.load(io)
    cm.body.should == true
  end
end
