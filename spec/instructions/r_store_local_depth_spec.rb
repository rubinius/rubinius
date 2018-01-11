require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_store_local_depth" do
  before do
    @spec = InstructionSpec.new :r_store_local_depth do |g|
      g.push_nil
      g.ret
    end
  end

  it "sets the local at slot and depth to the value of the register" do
    @spec.run.should be_nil
  end
end
