require File.expand_path("../spec_helper", __FILE__)

describe "Instruction send_vcall" do
  before do
    @spec = InstructionSpec.new :send_vcall do |g|
      g.push_nil
      g.ret
    end
  end

  it "<describe instruction effect>" do
    @spec.run.should be_nil
  end
end
