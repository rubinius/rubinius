require File.expand_path("../spec_helper", __FILE__)

describe "Instruction goto_if_nil" do
  before do
    @spec = InstructionSpec.new :goto_if_nil do |g|
      done = g.new_label

      g.push_literal :a
      g.push_nil
      g.goto_if_nil done

      g.pop
      g.push_literal :b

      done.set!
      g.ret
    end
  end

  it "branches to the location if the object at stack bottom is nil" do
    @spec.run.should == :a
  end
end
