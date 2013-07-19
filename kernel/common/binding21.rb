# -*- encoding: us-ascii -*-

class Binding
  def self.self_context(recv, variables)
    recv.equal?(Kernel) ? recv : variables.self
  end
  private :self_context
end
