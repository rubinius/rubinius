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
end
