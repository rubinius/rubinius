require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_load_stack" do
  before do
    @spec = InstructionSpec.new :r_load_stack do |g|
      g.push_literal "hello"

      r0 = g.new_register
      g.r_load_stack r0
      g.pop

      g.r_store_stack r0
      g.ret
    end
  end

  it "loads the value at the top of the stack into the register" do
    @spec.run.should == "hello"
  end
end
