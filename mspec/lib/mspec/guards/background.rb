require 'mspec/guards/guard'

# Some specs, notably those for Readline, will block under certain
# circumstances when run as background or subprocesses. Use this guard for
# such specs.

class BackgroundGuard < SpecGuard
  def match?
    MSpec.mode? :background
  end
end

class Object
  def process_is_foreground
    g = BackgroundGuard.new
    yield if g.yield? true
    g.unregister
  end
end
