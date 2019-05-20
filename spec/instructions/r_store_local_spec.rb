require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_store_local" do
  before do
    @spec = InstructionSpec.new :r_store_local do |g|
      g.local_count = 1

      g.push_int 3

      r0 = g.new_register
      g.r_load_stack r0

      g.r_store_local r0, 0

      g.push_local 0
      g.ret
    end
  end

  it "sets the local to the value in the register" do
    @spec.run.should == 3
  end
end
