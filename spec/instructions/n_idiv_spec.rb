require File.expand_path("../spec_helper", __FILE__)

describe "Instruction n_idiv" do
  before do
    @spec = InstructionSpec.new :n_idiv do |g|
      g.push_nil
      g.ret
    end
  end

  it "<describe instruction effect>" do
    @spec.run.should be_nil
  end
end
