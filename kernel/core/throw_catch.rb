class Object
  # FIXME - This totally shouldn't be a global.
  $__catch_stack__ ||= []

  def catch(sym)
    callcc do |cc|
      $__catch_stack__.push([sym, cc])
      yield
    end
  ensure
    $__catch_stack__.pop
  end

  def throw(sym, value = nil)
    until $__catch_stack__.empty? || $__catch_stack__.last[0] == sym
      $__catch_stack__.pop
    end
    raise NameError.new("uncaught throw `#{sym}'") if $__catch_stack__.empty?
    $__catch_stack__.last[1].call(value)
  end
end


