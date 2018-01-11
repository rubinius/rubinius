require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_copy" do
  before do
    @spec = InstructionSpec.new :r_copy do |g|
      r0 = g.new_register
      r1 = g.new_register

      g.push_int 10
      g.r_load_stack r0
      g.pop

      g.r_copy r1, r0

      g.r_store_stack r1
      g.ret
    end
  end

  it "copies the value in the second register to the first register" do
    @spec.run.should == 10
  end
end
