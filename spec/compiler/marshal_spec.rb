require File.dirname(__FILE__) + "/spec_helper"

describe Compiler::Marshal do
  before(:each) do
    @cm = Compiler::Marshal.new
  end

  it "marshals true" do
    @cm.marshal(true).should == "t\n"
  end

  it "unmarshals into true" do
    @cm.unmarshal("t\n").should == true
  end

  it "marshals false" do
    @cm.marshal(false).should == "f\n"
  end

  it "unmarshals false" do
    @cm.unmarshal("f\n").should == false
  end

  it "marshals nil" do
    @cm.marshal(nil).should == "n\n"
  end

  it "unmarshals into nil" do
    @cm.unmarshal("n\n").should == nil
  end

  it "marshals Fixnums" do
    @cm.marshal(3).should == "I\n3\n"
  end

  it "unmarshals into Fixnum" do
    @cm.unmarshal("I\n3\n").should == 3
  end

  it "marshals Bignum" do
    obj = 33242342342344234
    @cm.marshal(obj).should == "I\n33242342342344234\n"
  end

  it "unmarshals into Bignum" do
    @cm.unmarshal("I\n33242342342344234\n").should == 33242342342344234
  end

  it "marshals Strings" do
    @cm.marshal("blah").should == "s\n4\nblah\n"
  end

  it "unmarshals into String" do
    @cm.unmarshal("s\n4\nblah\n").should == "blah"
  end

  it "unmarshals into String containing newlines" do
    @cm.unmarshal("s\n6\nblah\na\n").should == "blah\na"
  end

  it "marshals Symbols" do
    @cm.marshal(:blah).should == "x\n4\nblah\n"
  end

  it "unmarshals into Symbol" do
    @cm.unmarshal("x\n4\nblah").should == :blah
  end

  it "marshals SendSite" do
    obj = SendSite.new(:blah)
    @cm.marshal(obj).should == "S\n4\nblah\n"
  end

  it "unmarshals a SendSite" do
    obj = @cm.unmarshal("S\n4\nblah\n")
    obj.should be_kind_of(SendSite)
    obj.name.should == :blah
  end

  it "marshals Arrays" do
    obj = [1,2,3]
    @cm.marshal(obj).should == "A\n3\nI\n1\nI\n2\nI\n3\n"
  end

  it "unmarshals an Array" do
    str = "A\n3\nI\n1\nI\n2\nI\n3\n"
    @cm.unmarshal(str).should == [1,2,3]
  end

  it "marshals Tuples" do
    obj = Tuple[1,2,3]
    @cm.marshal(obj).should == "p\n3\nI\n1\nI\n2\nI\n3\n"
  end
  
  it "unmarshals an Tuple" do
    str = "p\n3\nI\n1\nI\n2\nI\n3\n"
    obj = @cm.unmarshal(str)
    obj.should be_kind_of(Tuple)
    obj.should == Tuple[1,2,3]
  end

  it "marshals Floats" do
    @cm.marshal(15.5).should == "d\n15.5\n"
  end

  it "unmarshals into Float" do
    @cm.unmarshal("d\n15.5\n").should == 15.5
  end

  it "marshals InstructionSequences" do
    enc = InstructionSequence::Encoder.new
    iseq = enc.encode_stream([:noop])

    @cm.marshal(iseq).should == "i\n1\n0\n"
  end

  it "unmarshal into InstructionSequence" do
    iseq = @cm.unmarshal("i\n1\n0\n")
    iseq.should be_kind_of(InstructionSequence)
    iseq.opcodes.should == Tuple[0]
  end

  it "marshals LookupTables" do
    lt = LookupTable.new
    lt[:metadata] = 1

    @cm.marshal(lt).should == "l\n1\n8\nmetadata\nI\n1\n"
  end

  it "unmarshals into LookupTable" do
    str = "l\n1\n8\nmetadata\nI\n1\n"
    lt = @cm.unmarshal(str)
    lt.should be_kind_of(LookupTable)
    lt.keys.should == [:metadata]
    lt[:metadata].should == 1
  end

  it "marshals CompiledMethods" do
    cm = CompiledMethod.new
    cm.__ivars__ = nil
    cm.primitive = :blah
    cm.name = :test

    enc = InstructionSequence::Encoder.new
    cm.iseq = enc.encode_stream([:noop])

    cm.stack_size = 10
    cm.local_count = 0
    cm.required_args = 0
    cm.total_args = 0
    cm.splat = nil

    cm.literals = Tuple[1,2]

    cm.exceptions = nil
    cm.lines = Tuple[Tuple[0,1,1]]
    cm.file = :not_real
    cm.local_names = Tuple[:blah]

    @cm.marshal(cm).should == "M\n1\nn\nx\n4\nblah\nx\n4\ntest\ni\n1\n0\nI\n10\nI\n0\nI\n0\nI\n0\nn\np\n2\nI\n1\nI\n2\nn\np\n1\np\n3\nI\n0\nI\n1\nI\n1\nx\n8\nnot_real\np\n1\nx\n4\nblah\n"
  end

  it "unmarshal into CompiledMethod" do
    str = "M\n1\nn\nx\n4\nblah\nx\n4\ntest\ni\n1\n0\nI\n10\nI\n0\nI\n0\nI\n0\nn\np\n2\nI\n1\nI\n2\nn\np\n1\np\n3\nI\n0\nI\n1\nI\n1\nx\n8\nnot_real\np\n1\nx\n4\nblah\n"

    cm = @cm.unmarshal(str)
    cm.should be_kind_of(CompiledMethod)

    cm.__ivars__.should == nil
    cm.primitive.should == :blah
    cm.name.should == :test

    cm.iseq.opcodes.should == Tuple[0]
    cm.stack_size.should == 10
    cm.local_count.should == 0
    cm.required_args.should == 0
    cm.total_args.should == 0
    cm.splat.should == nil
    cm.literals.should == Tuple[1,2]

    cm.exceptions.should == nil
    cm.lines.should == Tuple[Tuple[0,1,1]]
    cm.file.should == :not_real
    cm.local_names == Tuple[:blah]
  end
end
