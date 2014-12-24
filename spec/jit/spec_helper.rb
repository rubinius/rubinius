require File.expand_path('../../spec_helper', __FILE__)

WARMUP_ITERATIONS = Rubinius::JIT.compile_threshold

class Object
  def warmup
    WARMUP_ITERATIONS.times { yield }
  end
end
