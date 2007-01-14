$:.unshift File.dirname(__FILE__)
require 'mri_target'
require 'rubinius_target'
require 'jruby_target'

# Example courtesy of nicksieger, many thanks!
class Spec::Runner::Context
  def before_context_eval
    case ENV['SPEC_TARGET']
    when /mri/
      @context_eval_module.include MRITarget
    when /jruby/
      @context_eval_module.include JRubyTarget
    else
      @context_eval_module.include RubiniusTarget
    end
  end
end
