require File.expand_path("../spec_helper", __FILE__)

describe "Instruction noop" do
  before do
    @spec = InstructionSpec.new :noop do |g|
      g.push_nil
      g.ret
    end
  end

  it "<describe instruction effect>" do
    @spec.run.should be_nil
  end
end
