# vim: filetype=ruby

$:.unshift "."
require 'spec/custom/mspec'

class MSpecScript
  set :capi, [
    'spec/ruby/optional/capi'
  ]

  set :command_line, [ 'spec/command_line' ]

  set :compiler, [ 'spec/compiler' ]

  set :build, [ 'spec/build' ]

  # Prepended to file names when resolving spec files. Enables
  # commands like 'bin/mspec core/array' to be equivalent to
  # 'bin/mspec spec/ruby/core/array'
  set :prefix, 'spec/ruby'

  # The default implementation to run the specs.
  set :target, 'bin/rbx'

  # Leave out MSpec lines from backtraces
  set :backtrace_filter, %r[(mspec/bin|mspec/lib/mspec)]

  # Enable language features
  MSpec.enable_feature :fork

  rbx = defined?(RUBY_ENGINE) and RUBY_ENGINE == "rbx"

  if RUBY_VERSION >= "1.9" or rbx
    MSpec.enable_feature :require_19
  end

  if RUBY_VERSION >= "1.9"
    MSpec.enable_feature :encoding
  end

  if Object.const_defined?(:Rubinius) && Rubinius.const_get(:Fiber)
    if Rubinius::Fiber::ENABLED
      ::Fiber = Rubinius::Fiber
      MSpec.enable_feature :fiber
      MSpec.enable_feature :fiber_library
      MSpec.enable_feature :generator
    end
  end
end
