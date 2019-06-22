require File.expand_path("../spec_helper", __FILE__)

describe "Instruction push_tagged_nil" do
  before do
    @spec = InstructionSpec.new :push_tagged_nil do |g|
      # Padding to create non-zero IP for push_tagged_nil
      g.noop
      g.noop

      g.push_tagged_nil 0
      g.ret
    end
  end

  it "adds nil to the bottom of the stack" do
    @spec.run.should be_nil
  end

  it "adds a value that is an instance of NilClass to the bottom of the stack" do
    @spec.run.should be_an_instance_of(NilClass)
  end

  it "adds a nil tagged with the CompiledCode ID to the bottom of the stack" do
    m = Rubinius::Mirror::Object.reflect(@spec.run)
    m.nil_code_id.should == @spec.code.code_id[0...8]
  end

  it "adds a nil tagged with the instruction address (IP) to the bottom of the stack" do
    m = Rubinius::Mirror::Object.reflect(@spec.run)
    m.nil_ip.should == 2
  end
end
