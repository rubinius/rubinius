require File.expand_path("../spec_helper", __FILE__)

describe "Instruction goto_if_undefined" do
  before do
    @spec = InstructionSpec.new :goto_if_undefined do |g|
      g.push_nil
      g.ret
    end
  end

  it "<describe instruction effect>" do
    @spec.run.should be_nil
  end
end
