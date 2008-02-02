require 'mspec/runner/mspec'
require 'mspec/runner/actions/tally'

class SpecGuard
  def self.register
    unless @registered
      @tally = TallyAction.new
      @tally.register
      MSpec.register :finish, self
      @registered = true
    end
  end
  
  def self.unregister
    @tally.unregister if @tally
  end
  
  def self.finish
  end
  
  def initialize(*args)
    @args = args
  end
  
  def yield?(invert=false)
    self.class.register
    if MSpec.report_mode?
      MSpec.register :before, self
      return true
    elsif MSpec.verify_mode?
      MSpec.register :after, self
      return true
    end
    return match? ^ invert
  end
  
  def match?
    false
  end
  
  def ===(other)
    true
  end
  
  def before(state)
  end
  
  def after(state)
  end
  
  def unregister
    MSpec.unregister :before, self
    MSpec.unregister :after, self
    MSpec.unregister :exclude, self
    self.class.unregister
  end
end
