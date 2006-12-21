require 'rubygems'
require 'spec'

$:.unshift File.dirname(__FILE__)
require 'mri_target'
require 'rubinius_target'

# Definitions
#
# spec: a statement of an expected behavior
# example: a segment of code that illustrates a spec and that can pass or fail an expectation
# host: the system (interpreter, vm, etc.) that will execute the spec frameworkd (e.g. RSpec)
# target: the system (interpreter , vm, etc.) on which the example will be executed

class SpecConfiguration
  Target = MRITarget
end

module Kernel
  def context_with_target(name, &block)
    context_without_target name do
      inherit SpecConfiguration::Target
      self.class_eval &block
    end
  end
  
  alias context_without_target context
  alias context context_with_target
end
