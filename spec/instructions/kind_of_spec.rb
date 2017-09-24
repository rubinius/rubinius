require File.expand_path("../spec_helper", __FILE__)

describe "Instruction kind_of" do
  before do
    @spec = InstructionSpec.new :kind_of do |g|
      g.push_nil
      g.ret
    end
  end

  it "<describe instruction effect>" do
    @spec.run.should be_nil
  end
end
