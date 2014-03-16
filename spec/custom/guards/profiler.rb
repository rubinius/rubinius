require 'rubinius/profiler'

class ProfilerGuard < SpecGuard
  def match?
    Rubinius::Profiler::Instrumenter.available? and
      not Rubinius::Profiler::Instrumenter.active?
  end
end

class Object
  def with_profiler
    g = ProfilerGuard.new
    g.name = :with_profiler
    yield if g.yield?
  ensure
    g.unregister
  end
end
