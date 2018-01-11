require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_load_local" do
  before do
    @spec = InstructionSpec.new :r_load_local do |g|
      g.local_count = 1

      g.push_int 12
      g.set_local 0

      r0 = g.new_register
      g.r_load_local r0, 0

      g.r_store_stack r0
      g.ret
    end
  end

  it "loads the value in the local into the register" do
    @spec.run.should == 12
  end
end
