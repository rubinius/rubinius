require 'tooling/profiler/profiler'

module Profiler__
  def start_profile
    @p = Rubinius::Profiler::Instrumenter.new
    @p.start
  end

  def stop_profile
    @p.stop
  end

  def options(opts)
    @p.set_options opts
  end

  def print_profile(f)
    stop_profile
    @p.show(f)
  end

  module_function :start_profile, :stop_profile, :print_profile, :options
end
