require File.expand_path('../../../../spec_helper', __FILE__)

describe "Encoder#replace_instruction" do
  before :each do
    @encoder = InstructionSequence::Encoder.new
  end

  it "replaces instruction with the same width" do
    orig = [:open_class, 0xAB]
    new = [:send_method, 0xCD]
    iseq = @encoder.encode_stream([orig, [:pop]])
    @encoder.replace_instruction(iseq, 0, new).should == orig
    @encoder.decode_iseq(iseq).first.should == new
  end

  it "replaces instruction with a smaller width" do
    orig = [:open_class, 0xAB]
    new = [:push_nil]
    iseq = @encoder.encode_stream([orig, [:pop]])
    @encoder.replace_instruction(iseq, 0, new).should == orig
    @encoder.decode_iseq(iseq)[0..1].should == [new, [:noop]]
  end

  it "replaces instruction with a larger width if followed by enough noop" do
    orig = [:send_method, 0xAB]
    new = [:check_argcount, 1, 2]
    iseq = @encoder.encode_stream([orig, [:noop]])
    @encoder.replace_instruction(iseq, 0, new).should == orig
    @encoder.decode_iseq(iseq).first.should == new
  end

  it "does not replace instruction with a larger width in general" do
    orig = [:send_method, 0xAB]
    new = [:check_argcount, 1, 2]
    iseq = @encoder.encode_stream([orig, [:pop]])
    lambda {
      @encoder.replace_instruction(iseq, 0, new)
    }.should raise_error(ArgumentError, "Cannot replace an instruction with a larger instruction (existing #{orig[0]} / new #{new[0]})")
  end
end
