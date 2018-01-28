require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_load_literal" do
  before do
    @spec = InstructionSpec.new :r_load_literal do |g|
      r0 = g.new_register

      g.r_load_literal r0, "hello"
      g.r_store_stack r0
      g.ret
    end
  end

  it "loads the literal at the index into the register" do
    @spec.run.should == "hello"
  end
end
