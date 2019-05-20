require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_store_int" do
  before do
    @spec = InstructionSpec.new :r_store_int do |g|
      r0 = g.new_register

      # We cannot work with the actual machine value 0 (which would be
      # considered by the machine as a pointer to memory address 0), so we
      # infer that the instruction works by computing a value from the value
      # it creates.

      # Start with the Fixnum 0, which is 1 for the machine
      g.push_int 0
      g.r_load_stack r0
      g.pop

      # Add 1 to set the value to 2 (ie 0b10)
      g.n_iadd r0, r0, r0

      # Tag the value to create 0b101 (ie Fixnum 2)
      g.r_store_int r0, r0

      g.r_store_stack r0
      g.ret
    end
  end

  it "stores the value of the second register into the first register after apply the Fixnum tag" do
    @spec.run.should == 2
  end
end
