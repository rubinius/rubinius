require File.expand_path("../spec_helper", __FILE__)

describe "Instruction yield_stack" do
  before do
    @spec = InstructionSpec.new :yield_stack do |g|
      g.push_nil
      g.ret
    end
  end

  it "<describe instruction effect>" do
    @spec.run.should be_nil
  end
end
