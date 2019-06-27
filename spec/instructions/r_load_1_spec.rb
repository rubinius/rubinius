require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_load_1" do
  before do
    @spec = InstructionSpec.new :r_load_1 do |g|
      r0 = g.new_register

      g.r_load_1 r0
      g.r_store_int r0, r0
      g.r_store_stack r0
      g.ret
    end
  end

  it "loads the untagged value 1 in the register" do
    @spec.run.should == 1
  end
end
