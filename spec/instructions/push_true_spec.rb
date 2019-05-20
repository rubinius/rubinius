require File.expand_path("../spec_helper", __FILE__)

describe "Instruction push_true" do
  before do
    @spec = InstructionSpec.new :push_true do |g|
      g.push_true
      g.ret
    end
  end

  it "adds true to the bottom of the stack" do
    @spec.run.should be_true
  end
end
