require 'spec/custom/utils/options'

if defined?(RUBY_ENGINE) && RUBY_ENGINE == 'rbx'
  require 'spec/custom/matchers/compile_as'
  require 'spec/custom/helpers/generator'
  require 'spec/custom/guards/profiler'
  require 'spec/custom/runner/relates'
  require 'spec/custom/runner/actions/debug'
  require 'spec/custom/runner/actions/gcstats'
  require 'spec/custom/runner/actions/memory'
  require 'spec/custom/runner/actions/parser'
  require 'spec/custom/runner/actions/profiler'
end

require 'spec/custom/utils/script'
