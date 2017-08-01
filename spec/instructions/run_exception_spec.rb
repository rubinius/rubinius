require File.expand_path("../spec_helper", __FILE__)

describe "Instruction run_exception" do
  before do
    @spec = InstructionSpec.new :run_exception do |g|
      g.push_self
      g.send_method :m
      g.pop
      g.push_literal :a
      g.ret
      g.run_exception
    end

    def @spec.m
      raise RuntimeError
    end
  end

  it "propagates an un-rescued exception" do
    lambda { @spec.run }.should raise_error(RuntimeError)
  end
end

describe "Instruction run_exception" do
  before do
    @spec = InstructionSpec.new :run_exception do |g|
      ex = g.new_label
			done = g.new_label
      rescued = g.new_label
      unrescued = g.new_label

      exs1 = g.new_stack_local
      exs2 = g.new_stack_local

      g.push_exception_state
      g.set_stack_local exs1
      g.pop
      g.setup_unwind ex, 0
      g.push_self
      g.send_method :m
      g.pop_unwind
      g.goto done

      ex.set!
      g.push_exception_state
      g.set_stack_local exs2
      g.pop
      g.push_current_exception
      g.dup
      g.push_const :RuntimeError
      g.swap
      g.send_stack :===, 1
      g.goto_if_true rescued
      g.goto unrescued

      rescued.set!
      g.pop
      g.push_literal :a
      g.clear_exception
      g.goto done

      unrescued.set!
      g.pop
      g.push_stack_local exs2
      g.restore_exception_state
      g.reraise

      done.set!
      g.push_stack_local exs1
      g.restore_exception_state
      g.ret
      g.run_exception
    end

    def @spec.m
      raise RuntimeError
    end
  end

  it "does not propagate a rescued exception" do
    @spec.run.should == :a
  end
end
