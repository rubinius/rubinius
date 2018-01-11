require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_load_int" do
  before do
    @spec = InstructionSpec.new :r_load_int do |g|
      r0 = g.new_register
      r1 = g.new_register

      # We cannot work with the actual machine value 0 (which would be
      # considered by the machine as a pointer to memory address 0), so we
      # infer that the instruction works by computing a value from the value
      # it creates.

      # Start with the Fixnum 0, which is 1 for the machine
      g.push_int 0
      g.r_load_stack r0
      g.pop

      # Copy the literal value 1 into r1
      g.r_copy r1, r0

      # Untag the value of r0 and store it in r0
      g.r_load_int r0, r0

      # Add 1 to the value in r0 (which should be 0) to yield 3 (Fixnum 1)
      g.n_iadd r0, r0, r1
      g.n_iadd r0, r0, r1
      g.n_iadd r0, r0, r1

      g.r_store_stack r0
      g.ret
    end
  end

  it "stores the untagged value of the second register in the first register" do
    @spec.run.should == 1
  end
end
