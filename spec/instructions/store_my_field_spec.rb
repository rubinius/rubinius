require File.expand_path("../spec_helper", __FILE__)

describe "Instruction store_my_field" do
  before do
    @spec = InstructionSpec.new :store_my_field do |g|
      g.push_nil
      g.ret
    end
  end

  it "<describe instruction effect>" do
    @spec.run.should be_nil
  end
end
