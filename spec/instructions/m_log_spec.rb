require File.expand_path("../spec_helper", __FILE__)

describe "Instruction m_log" do
  before do
    @spec = InstructionSpec.new :m_log do |g|
      r0 = g.new_register

      g.r_load_literal r0, "test output for m_log instruction"
      g.m_log r0
      g.r_store_stack r0
      g.ret
    end
  end

  it "<describe instruction effect>" do
    @spec.run.should == "test output for m_log instruction"
  end
end

