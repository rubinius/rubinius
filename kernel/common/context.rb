# depends on: class.rb

##
# Stores all information about a running Block.

class BlockContext

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

  def home
    env.home
  end

  def name
    home.name
  end

  def receiver
    home.receiver
  end

  def method_module
    home.method_module
  end

  def current_scope
    home.current_scope
  end
end
