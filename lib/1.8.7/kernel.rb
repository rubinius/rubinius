module Kernel
  def __method__
    # Look in the backtrace for locations where the scope is that of the sender.
    # The first non-block one will be our call,
    # unless where are called from a DelegatedMethod
    # For the later, we check to see if the next location
    # was a call from a delegated method
    scope_of_sender = Rubinius::StaticScope.of_sender
    trace = Rubinius::VM.backtrace(1)
    trace.each_with_index do |loc, i|
      next unless loc.method.scope == scope_of_sender
      if loc.is_block
        delegated = trace.at(i+1).delegated_method_name
        return delegated if delegated
      else
        return loc.method.name == :__script__ ? nil : loc.method.name
      end
    end
    nil
  end
end

module Rubinius
  class Location
    def delegated_method_name
      return @receiver.name if @receiver.class == DelegatedMethod
    end
  end
end