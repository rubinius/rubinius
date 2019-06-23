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

describe "Instruction ret" do
  before do
    @spec = InstructionSpec.new :ret do |g|
      g.push_literal :a
      g.push_nil
      g.ret
    end
  end

  it "tags a nil value with the CompiledCode ID if it is not already tagged" do
    m = Rubinius::Mirror::Object.reflect(@spec.run)
    m.nil_code_id.should == @spec.code.code_id[0...8]
  end
end
