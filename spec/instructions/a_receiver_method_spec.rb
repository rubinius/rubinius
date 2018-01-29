require File.expand_path("../spec_helper", __FILE__)

describe "Instruction a_receiver_method" do
  before do
    @spec = InstructionSpec.new :a_receiver_method do |g|
      g.push_nil
      g.ret
    end
  end

  it "<describe instruction effect>" do
    @spec.run.should be_nil
  end
end
