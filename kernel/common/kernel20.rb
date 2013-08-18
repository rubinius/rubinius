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

  def Hash(obj)
    return {} if obj.nil? || obj == []

    if hash = Rubinius::Type.check_convert_type(obj, Hash, :to_hash)
      return hash
    end

    raise TypeError, "can't convert #{obj.class} into Hash"
  end
  module_function :Hash
end
