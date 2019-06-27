require File.expand_path("../spec_helper", __FILE__)

describe "Instruction goto_past" do
  before do
    @spec = InstructionSpec.new :goto_past do |g|
      done = g.new_label
      past = g.new_label
      future = g.new_label

      g.push_int 24
      g.goto future

      past.set!
      g.push_int 42
      g.goto done

      future.set!
      g.pop
      g.goto_past past

      done.set!
      g.ret
    end
  end

  it "goes to a location before the current instruction" do
    @spec.run.should == 42
  end
end
