require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_store_stack" do
  before do
    @spec = InstructionSpec.new :r_store_stack do |g|
      r0 = g.new_register

      g.push_int 10
      g.r_load_stack r0

      g.n_iadd_o r0, r0, r0

      g.r_store_stack r0
      g.ret
    end
  end

  it "pushes the value in the register onto the stack" do
    @spec.run.should == 20
  end
end
