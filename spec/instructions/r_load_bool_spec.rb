require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_load_bool" do
  before do
    @spec = InstructionSpec.new :r_load_bool do |g|
      g.required_args = 1
      g.total_args = 1
      g.local_count = 1

      r0 = g.new_register
      r1 = g.new_register

      g.r_load_local r0, 0

      g.r_load_int r0, r0
      g.r_load_bool r1, r0

      g.r_store_stack r1
      g.ret
    end
  end

  it "loads true in the register if the value of the register is non-zero" do
    @spec.run(42).should be_true
  end

  it "loads false in the register if the value of the register is zero" do
    @spec.run(0).should be_false
  end
end
