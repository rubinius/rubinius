require File.expand_path("../spec_helper", __FILE__)

describe "Instruction a_equal" do
  before do
    @spec = InstructionSpec.new :a_equal do |g|
      r0 = g.new_register
      r1 = g.new_register

      g.r_load_literal r0, "a"
      g.r_copy r1, r0
      g.a_equal r0, r1

      g.push_nil
      g.ret
    end
  end

  it "does not raise an exception when the arguments are identically equal" do
    @spec.run.should be_nil
  end
end

describe "Instruction a_equal" do
  before do
    @spec = InstructionSpec.new :a_equal do |g|
      r0 = g.new_register
      r1 = g.new_register

      g.r_load_literal r0, :a
      g.r_load_literal r1, "a"
      g.a_equal r0, r1

      g.push_nil
      g.ret
    end
  end

  it "raises an AssertionError when the arguments are not identically equal" do
    lambda { @spec.run }.should raise_error(Rubinius::AssertionError)
  end
end
