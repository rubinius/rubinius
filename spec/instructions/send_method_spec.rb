require File.expand_path("../spec_helper", __FILE__)

describe "Instruction send_method" do
  before do
    @spec = InstructionSpec.new :send_method do |g|
      g.push_nil
      g.ret
    end
  end

  it "<describe instruction effect>" do
    @spec.run.should be_nil
  end
end
