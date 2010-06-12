$: << File.expand_path('../', __FILE__)

require 'spec/ruby/spec_helper'

if Object.const_defined?(:RUBY_ENGINE) and Object.const_get(:RUBY_ENGINE) == 'rbx'
  require 'spec/custom/mspec'
end
