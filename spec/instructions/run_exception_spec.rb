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

describe "Instruction run_exception" do
  before do
    @spec = InstructionSpec.new :run_exception do |g|
			ex = g.new_label
			no_exc = g.new_label

      exs = g.new_stack_local

      g.setup_unwind ex, 1
      g.push_exception_state
      g.set_stack_local exs
      g.pop
      g.push_self
      g.send_method :m
      g.pop_unwind
      g.goto no_exc

      ex.set!
      g.push_exception_state
      g.push_self
      g.send_method :n
      g.pop
      g.restore_exception_state
      g.reraise

      no_exc.set!
      g.push_self
      g.send_method :n
      g.pop
      g.ret
      g.run_exception
    end

    def @spec.m
      raise RuntimeError
    end
  end

  it "runs ensure blocks when an exception is raised" do
    @spec.should_receive(:n)

    lambda { @spec.run }.should raise_error(RuntimeError)
  end
end

describe "Instruction run_exception" do
  before do
    @spec = InstructionSpec.new :run_exception do |g|
			ex = g.new_label
			no_exc = g.new_label

      exs = g.new_stack_local

      g.setup_unwind ex, 1
      g.push_exception_state
      g.set_stack_local exs
      g.pop
      g.push_self
      g.send_method :m
      g.pop_unwind
      g.goto no_exc

      ex.set!
      g.push_exception_state
      g.push_self
      g.send_method :n
      g.pop
      g.restore_exception_state
      g.reraise

      no_exc.set!
      g.push_self
      g.send_method :n
      g.pop
      g.ret
      g.run_exception
    end

    def @spec.m
      :a
    end
  end

  it "runs ensure blocks when no exception is raised" do
    @spec.should_receive(:n)

    @spec.run.should == :a
  end
end

describe "Instruction run_exception" do
  before do
    @spec = InstructionSpec.new :run_exception do |g|
      blk = g.class.new
      blk.name = :__block__
      blk.set_line 1

      blk.push_literal :a
      blk.raise_break
      blk.pop
      blk.push_literal :b
      blk.ret
      blk.run_exception
      blk.close

      g.push_self
      g.create_block blk
      g.send_stack_with_block :m, 0
      g.ret
      g.run_exception
    end

    def @spec.m
      yield
    end
  end

  it "continues running the method when a block breaks" do
    @spec.code.literals[0].tags |= Rubinius::Executable::Experimental
    @spec.run.should == :a
  end
end
