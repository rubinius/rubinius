# -*- encoding: us-ascii -*-

module Kernel
  private :initialize_dup
  private :initialize_clone
  private :respond_to_missing?
  public :remove_instance_variable

  def warn(*messages)
    $stderr.puts(*messages) if !$VERBOSE.nil? && !messages.empty?
    nil
  end
  module_function :warn
end
