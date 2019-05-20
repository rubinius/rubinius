require File.expand_path("../spec_helper", __FILE__)

describe "Instruction push_nil" do
  before do
    @spec = InstructionSpec.new :push_nil do |g|
      g.push_nil
      g.ret
    end
  end

  it "adds nil to the bottom of the stack" do
    @spec.run.should be_nil
  end
end
