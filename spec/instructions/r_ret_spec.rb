require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_ret" do
  before do
    @spec = InstructionSpec.new :r_ret do |g|
      r0 = g.new_register

      g.r_load_1 r0
      g.r_store_int r0, r0
      g.r_ret r0
    end
  end

  it "returns the value in the register" do
    @spec.run.should equal(1)
  end
end
