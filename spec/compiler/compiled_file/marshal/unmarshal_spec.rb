require File.expand_path('../../../../spec_helper', __FILE__)

describe "Rubinius::CompiledFile::Marshal#unmarshal" do
  it "unmarshals true" do
    marshal = Rubinius::CompiledFile::Marshal.new
    marshal.unmarshal("t\n").should == true
  end

  it "unmarshals false" do
    marshal = Rubinius::CompiledFile::Marshal.new
    marshal.unmarshal("f\n").should == false
  end

  it "unmarshals nil" do
    marshal = Rubinius::CompiledFile::Marshal.new
    marshal.unmarshal("n\n").should == nil
  end

  it "unmarshals an Integer" do
    marshal = Rubinius::CompiledFile::Marshal.new
    marshal.unmarshal("I\nCAFEF00D").should == 0xCAFEF00D
  end

  it "unmarshals a Float" do
    marshal = Rubinius::CompiledFile::Marshal.new
    marshal.unmarshal("d\n +0.785000000000000031086244689504383131861686706542968750     2\n").should == 3.14
  end

  it "unmarshals +infinity" do
    marshal = Rubinius::CompiledFile::Marshal.new
    marshal.unmarshal("d\nInfinity\n").should be_positive_infinity
  end

  it "unmarshals -infinity" do
    marshal = Rubinius::CompiledFile::Marshal.new
    marshal.unmarshal("d\n-Infinity\n").should be_negative_infinity
  end

  it "unmarshals NaN" do
    marshal = Rubinius::CompiledFile::Marshal.new
    marshal.unmarshal("d\nNaN\n").should be_nan
  end

  it "unmarshals a Symbol" do
    marshal = Rubinius::CompiledFile::Marshal.new
    marshal.unmarshal("x\nE\n8\nUS-ASCII\n12\nHello world!\n").should == :"Hello world!"
  end

  it "unmarshals a constant" do
    marshal = Rubinius::CompiledFile::Marshal.new
    marshal.unmarshal("c\n22\nRubinius::CompiledFile\n").should == Rubinius::CompiledFile
  end

  ruby_version_is ""..."1.9" do
    it "unmarshals a String" do
      marshal = Rubinius::CompiledFile::Marshal.new
      marshal.unmarshal("s\nE\n0\n\n12\nHello world!\n").should == "Hello world!"
    end

    it "returns nil when unmarshalling an encoding" do
      marshal = Rubinius::CompiledFile::Marshal.new
      marshal.unmarshal("E\n5\nUTF-8\n13\n").should be_nil
    end
  end

  ruby_version_is "1.9" do
    it "unmarshals a String" do
      marshal = Rubinius::CompiledFile::Marshal.new
      str = marshal.unmarshal("s\nE\n5\nUTF-8\n12\nHello world!\n")
      str.should == "Hello world!"
      str.encoding.should == Encoding::UTF_8
    end

    it "unmarshals an encoding" do
      marshal = Rubinius::CompiledFile::Marshal.new
      marshal.unmarshal("E\n5\nUTF-8\n13\n").should == Encoding::UTF_8
    end
  end
end
