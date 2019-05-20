require File.expand_path("../spec_helper", __FILE__)

describe "Instruction shift_array" do
  before do
    @spec = InstructionSpec.new :shift_array do |g|
      g.push_nil
      g.ret
    end
  end

  it "<describe instruction effect>" do
    @spec.run.should be_nil
  end
end
