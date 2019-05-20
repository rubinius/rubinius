require File.expand_path("../spec_helper", __FILE__)

describe "Instruction a_less" do
  before do
    @spec = InstructionSpec.new :a_less do |g|
      r0 = g.new_register
      r1 = g.new_register

      g.r_load_literal r0, "1"
      g.r_load_literal r1, 1
      g.a_less r0, r1

      g.push_nil
      g.ret
    end
  end

  it "raises an AssertionError when the first value is not an Integer" do
    lambda { @spec.run }.should raise_error(Rubinius::AssertionError)
  end
end

describe "Instruction a_less" do
  before do
    @spec = InstructionSpec.new :a_less do |g|
      r0 = g.new_register
      r1 = g.new_register

      g.r_load_literal r0, 1
      g.r_load_literal r1, "1"
      g.a_less r0, r1

      g.push_nil
      g.ret
    end
  end

  it "raises an AssertionError when the second value is not an Integer" do
    lambda { @spec.run }.should raise_error(Rubinius::AssertionError)
  end
end

describe "Instruction a_less" do
  before do
    @spec = InstructionSpec.new :a_less do |g|
      r0 = g.new_register
      r1 = g.new_register

      g.r_load_literal r0, 1
      g.r_load_literal r1, 2
      g.a_less r0, r1

      g.push_nil
      g.ret
    end
  end

  it "does not raise an exception if the first value is less than the second" do
    @spec.run.should be_nil
  end
end

describe "Instruction a_less" do
  before do
    @spec = InstructionSpec.new :a_less do |g|
      r0 = g.new_register
      r1 = g.new_register

      g.r_load_literal r0, 41
      g.r_load_literal r1, 2
      g.a_less r0, r1

      g.push_nil
      g.ret
    end
  end

  it "raises an AssertionError when the first value is not less than the second" do
    lambda { @spec.run }.should raise_error(Rubinius::AssertionError)
  end
end
