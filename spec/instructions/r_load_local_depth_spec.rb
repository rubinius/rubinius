require File.expand_path("../spec_helper", __FILE__)

describe "Instruction r_load_local_depth" do
  before do
    @spec = InstructionSpec.new :r_load_local_depth do |g|
      g.push_nil
      g.ret
    end
  end

  it "loads the local at slot and depth into the register" do
    @spec.run.should be_nil
  end
end
