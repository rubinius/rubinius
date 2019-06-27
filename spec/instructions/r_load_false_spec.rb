require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_load_false" do
  before do
    @spec = InstructionSpec.new :r_load_false do |g|
      r0 = g.new_register

      g.r_load_false r0
      g.r_store_stack r0
      g.ret
    end
  end

  it "loads false in the register" do
    @spec.run.should be_false
  end
end
