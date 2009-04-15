# Custom MSpec action that prints profiler stats after a spec run
#
class ProfilerAction
  def register
    MSpec.register :start,  self
    MSpec.register :finish, self
  end

  def start
    @profiler = Rubinius::Profiler::Instrumenter.new(
      :sort => [:self_seconds, :calls])
    @profiler.start
  end

  def finish
    @profiler.stop
    @profiler.show
  end
end
