require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_load_m_binops" do
  before do
    @spec = InstructionSpec.new :r_load_m_binops do |g|
      g.required_args = 1
      g.total_args = 1
      g.local_count = 1

      r0 = g.new_register
      r1 = g.new_register

      g.r_load_m_binops r0, r1

      g.r_store_stack r0
      g.r_store_stack r1
      g.make_array 2

      g.ret
    end
  end

  it "loads self and the first argument into registers" do
    @spec.run(42).should == [@spec, 42]
  end
end
