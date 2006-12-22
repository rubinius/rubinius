$:.unshift File.dirname(__FILE__)
require 'mri_target'
require 'rubinius_target'

# Example courtesy of nicksieger, many thanks!
class Spec::Runner::Context
  def before_context_eval
    case ENV['SPEC_TARGET']
    when /mri/
      @context_eval_module.include MRITarget
    when /jruby/
      # that's right folks, we share
    else
      @context_eval_module.include RubiniusTarget
    end
  end
end
