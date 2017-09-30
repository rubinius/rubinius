require File.expand_path("../spec_helper", __FILE__)

describe "Instruction ret" do
  before do
    @spec = InstructionSpec.new :ret do |g|
      g.push_nil
      g.push_literal :a
      g.ret
      g.push_literal :b
      g.ret
    end
  end

  it "returns the value at the top of the stack" do
    @spec.run.should equal(:a)
  end
end
