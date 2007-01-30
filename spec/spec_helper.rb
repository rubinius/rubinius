$:.unshift File.dirname(__FILE__)

class String
  # remove indentation from the beginning of each line
  # according to the number of blanks in the first line
  def unindent
    match = /^\s+/.match(self)
    return unless match
    self.gsub(/^\s{#{match[0].length}}/, "")
  end
end

# Example courtesy of nicksieger, many thanks!
class Spec::Runner::Context
  def before_context_eval
    case ENV['SPEC_TARGET']
    when /mri/
      require 'mri_target'
      @context_eval_module.include MRITarget
    when /jruby/
      require 'jruby_target'
      @context_eval_module.include JRubyTarget
    else
      require 'rubinius_target'
      @context_eval_module.include RubiniusTarget
    end
  end
end

# constants for coerce tests
FIXNUM  = 1
FIXNUM2 = 2
FLOAT   = 3.0
FLOAT2  = 4.0
BIGNUM  = 23423429034820348230947247239472394723984723984723984723984729384723984
BIGNUM2 = 13423429034837733330947247239472394723984723984723984723984729384723984

