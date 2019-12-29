require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_load_f_binops" do
  before do
    @spec = InstructionSpec.new :r_load_f_binops do |g|
      g.required_args = 2
      g.total_args = 2
      g.local_count = 2

      r0 = g.new_register
      r1 = g.new_register

      g.r_load_f_binops r0, r1

      g.r_store_stack r0
      g.r_store_stack r1
      g.make_array 2

      g.ret
    end
  end

  it "loads first two argument into registers" do
    @spec.run(42, 24).should == [42, 24]
  end
end
