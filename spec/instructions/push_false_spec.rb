require File.expand_path("../spec_helper", __FILE__)

describe "Instruction push_false" do
  before do
    @spec = InstructionSpec.new :push_false do |g|
      g.push_false
      g.ret
    end
  end

  it "adds false to the bottom of the stack" do
    @spec.run.should be_false
  end
end
