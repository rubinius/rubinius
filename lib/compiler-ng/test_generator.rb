# TODO: document TestGenerator
require 'spec/custom/helpers/generator'

class TestGenerator
  attr_accessor :name, :file, :required_args, :total_args, :splat_index,
                :local_count, :local_names, :primitive, :for_block

  def state
    @state.last
  end

  # TODO: put TestGenerator under Rubinius
  def push_state(scope)
    @state << Rubinius::AST::State.new(scope)
  end

  def pop_state
    @state.pop
  end

  # TODO: fix TestGenerator to reflect current setup_unwind insn
  def setup_unwind(label, *)
    add :setup_unwind, label
  end

  def set_line(line)
    @line = line
  end
end
