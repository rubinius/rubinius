require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_load_nil" do
  before do
    @spec = InstructionSpec.new :r_load_nil do |g|
      r0 = g.new_register

      g.r_load_nil r0, 0
      g.r_store_stack r0
      g.ret
    end
  end

  it "loads nil in the register" do
    @spec.run.should be_nil
  end

  it "loads a value that is an instance of NilClass in the register" do
    @spec.run.should be_an_instance_of(NilClass)
  end
end

describe "Instruction r_load_nil" do
  before do
    @spec = InstructionSpec.new :r_load_nil do |g|
      r0 = g.new_register

      # Padding to create non-zero IP for r_load_nil
      g.noop
      g.noop

      g.r_load_nil r0, 0
      g.r_store_stack r0
      g.ret
    end
  end

  it "loads a nil tagged with the CompiledCode ID" do
    m = Rubinius::Mirror::Object.reflect(@spec.run)
    m.nil_code_id.should == @spec.code.code_id[0...8]
  end

  it "loads a nil tagged with the instruction address (IP)" do
    m = Rubinius::Mirror::Object.reflect(@spec.run)
    m.nil_ip.should == 2
  end
end
