require File.expand_path("../spec_helper", __FILE__)

describe "Instruction cast_multi_value" do
  before do
    @spec = InstructionSpec.new :cast_multi_value do |g|
      g.push_nil
      g.ret
    end
  end

  it "<describe instruction effect>" do
    @spec.run.should be_nil
  end
end
