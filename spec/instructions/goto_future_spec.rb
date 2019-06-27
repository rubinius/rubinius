require File.expand_path("../spec_helper", __FILE__)

describe "Instruction goto_future" do
  before do
    @spec = InstructionSpec.new :goto_future do |g|
      done = g.new_label
      past = g.new_label
      future = g.new_label

      g.push_int 24
      g.goto_future future

      past.set!
      g.pop
      g.push_int 42
      g.goto done

      future.set!
      g.goto done

      done.set!
      g.ret
    end
  end

  it "goes to a location after the current instruction" do
    @spec.run.should == 24
  end
end
