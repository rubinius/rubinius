require File.expand_path("../spec_helper", __FILE__)

describe "Instruction push_has_block" do
  before do
    @spec = InstructionSpec.new :push_has_block do |g|
      g.push_nil
      g.ret
    end
  end

  it "<describe instruction effect>" do
    @spec.run.should be_nil
  end
end
