module Profiler__
  def start_profile
    @p = Sampler.new
    @p.start
  end
  
  def stop_profile
    @p.stop
  end
  
  def print_profile(f)
    stop_profile
    @p.display(f)
  end
  
  module_function :start_profile, :stop_profile, :print_profile
end
