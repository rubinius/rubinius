require File.expand_path('../../spec_helper', __FILE__)

RubiniusJITSync = Rubinius::JIT.sync
Rubinius::JIT.sync = true

WARMUP_ITERATIONS = (ENV["RBX_JIT_SPEC_ITERATIONS"] ||
                     Rubinius::JIT.compile_threshold).to_i

class Object
  def warmup
    WARMUP_ITERATIONS.times { yield }
  end
end
