# depends on: method_context.rb block_environment.rb

##
# Stores all information about a running BlockEnvironment.
#
# A BlockContext contains a home and a BlockEnvironment in addition to a
# MethodContext.  The home points to the MethodContext that encloses the
# block which is used to find self, etc.

class BlockContext < MethodContext

  def from_eval?
    self.env.from_eval?
  end

  def last_match
    home.last_match
  end

  def last_match=(match)
    home.last_match = match
  end

  def nth_ref(idx)
    home.nth_ref(idx)
  end

  def back_ref(idx)
    home.back_ref(idx)
  end

  def method_visibility
    @method_visibility || env.home_block.method_visibility
  end

  ##
  # Active context (instance of MethodContext) that started
  # execution of this block context.

  def home
    env.home
  end

  ##
  # Name of home method.

  def name
    home.name
  end

  ##
  # Block context has no receiver thus uses receiver from it's home method
  # context.

  def receiver
    home.receiver
  end

  ##
  # Block context has no own module thus uses module from it's home method
  # context.

  def method_module
    home.method_module
  end

  ##
  # Static scope of home method context.

  def current_scope
    home.current_scope
  end

  # TODO: this version of current_scope was being shadowed, but causes
  # spec failures
  # def current_scope
  #   if ss = method.scope
  #     return ss.module
  #   else
  #     home.current_scope
  #   end
  # end

  ##
  # instance_eval needs alternate const behavior

  def __const_set__(name, value)
    const_scope = env.constant_scope.module
    const_scope.__send__(:__const_set__, name, value)
  end

  ##
  # emitted by Kernel#eval'd expressions where new locals
  # are introduced.

  def set_eval_local(name, val)
    home.set_eval_local name, val
  end

  ##
  # emitted by Kernel#eval'd expressions to retrieve a
  # dynamicly introduced local.

  def get_eval_local(name)
    home.get_eval_local(name)
  end
end

