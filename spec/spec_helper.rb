$:.unshift File.dirname(__FILE__)

begin
  if ENV['USE_RSPEC'] == '1'
    require 'rspec_helper'
    ExpectationNotMetError = Spec::Expectations::ExpectationNotMetError
  else
    require 'mspec_helper'
  end
rescue
  require 'mspec_helper'
end

require 'mini_mock'

# used for many should_be_close specs
TOLERANCE = 0.00003 unless Object.const_defined?(:TOLERANCE)

if !defined?(RUBY_NAME) then
  begin
    require 'rbconfig'
    RUBY_NAME = Config::CONFIG["RUBY_INSTALL_NAME"]
  rescue Exception
    RUBY_NAME = RUBY_ENGINE
  end
end

def engine?(name)
  case name
  when :rbx, :rubinius
    RUBY_NAME == 'rbx'
  when :mri, :ruby
    RUBY_NAME == 'ruby'
  when :jruby
    RUBY_NAME == 'jruby'
  else
    false
  end
end

# The four methods #compliant, #noncompliant, #extension, and #failure
# each describe one aspect of the specs they annotate, as follows:
#
# compliant(:ruby, :jruby) do
#   Run these specs on the listed engines. 
#   Used for specs that conform to the Ruby "standard"
# end
#
# noncompliant(:rubinius, :jruby) do
#   Run these specs on the listed engines.
#   Used for specs that do not conform to the Ruby "standard"
# end
#
# extension(:rubinus, :cardinal) do
#   Run these specs on the listed engines.
#   Used for specs that do NOT behave differently than a Ruby
#   "standard" behavior, perhaps because no equivalent behavior
#   exists in Ruby.
# end
#
# failure(:rubinius) do
#   Do NOT run these specs on the listed platform.
#   Used for specs that are expected to fail, perhaps because of
#   a bug on the listed engine. This is not for specs that are
#   noncompliant. This is a convenience to prevent executing
#   specs on a particular engine, perhaps because executing
#   the spec causes particularly nasty behavior.
# end
#
# Please refer to these comments for the semantics of these
# four methods. Don't refer to the implementation as it could
# likely change.


def compliant(*engines)
  if engines.any? { |e| engine?(e) }
    yield
  end
end

def failure(*engines)
  unless engines.any? { |e| engine?(e) }
    yield
  end
end

class Object
  alias noncompliant compliant
  alias extension compliant
end

class Object
  def should_include(other)
    unless self.include?(other)
      raise ExpectationNotMetError.new("Expected " + self.inspect + " to include " + other.inspect)
    end
  end

  def should_be_close(value, tolerance)
    unless (value - self).abs <= tolerance
      raise ExpectationNotMetError.new("Expected " + self.inspect + " to be close to " + value.inspect)
    end
  end
  
  def should_be_ancestor_of(klass)
    unless klass.ancestors.include?(self)
      raise ExpectationNotMetError.new("Expected " + self.class.name + " to be kind of " + klass.name)
    end
  end
end

def shared(msg, &block)
  return Proc.new(&block)
end

def it_behaves_like(behavior, meth)
  behavior.call(meth)
end
