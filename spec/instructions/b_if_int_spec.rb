require File.expand_path("../spec_helper", __FILE__)

describe "Instruction b_if_int" do
  before do
    @spec = InstructionSpec.new :b_if_int do |g|
      g.required_args = 2
      g.total_args = 2
      g.local_count = 2

      r0 = g.new_register
      r1 = g.new_register

      g.r_load_local r0, 0
      g.r_load_local r1, 1

      t = g.new_label
      done = g.new_label

      g.b_if_int r0, r1, t
      g.r_load_false r0
      g.goto done

      t.set!
      g.r_load_true r0

      done.set!
      g.r_store_stack r0

      g.ret
    end
  end

  it "branches to the label if the arguments are machine ints" do
    @spec.run(40, 2).should be_true
  end

  it "does not branch if the first argument is not a machine int" do
    @spec.run(4.0, 2).should be_false
  end

  it "does not branch if the second argument is not a machine int" do
    @spec.run(40, 2.0).should be_false
  end
end
