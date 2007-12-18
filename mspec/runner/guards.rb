unless defined?(RUBY_NAME)
  if defined?(RUBY_ENGINE)
    RUBY_NAME = RUBY_ENGINE
    if defined?(ARG0)
      if /rubinius.bin/.match(ARG0)
        RUBY_CLI = "shotgun/rubinius"
      else
        RUBY_CLI = ARG0
      end
    else
      RUBY_CLI = RUBY_NAME
    end
  else
    require 'rbconfig'
    RUBY_NAME = Config::CONFIG["RUBY_INSTALL_NAME"]
    RUBY_CLI =  RUBY_NAME
  end
end

module MSpec
  def self.engine?(name)
    !!case name
    when :rbx, :rubinius
      RUBY_NAME =~ /^rbx/
    when :ruby
      RUBY_NAME =~ /^ruby(1\.[^9])?/
    when :ruby19
      RUBY_NAME == 'ruby1.9'
    when :jruby
      RUBY_NAME =~ /^jruby/
    else
      false
    end
  end
  
  def self.guard?(*args, &cond)
    unless args.empty?
      reverse = args.first == :not
      args.shift if reverse
      result = args.any? &cond
    else
      result = yield
    end
    reverse ? (not result) : result
  end
  
  def self.runner?(runner)
    case runner
    when :mspec
      ENV['MSPEC_RUNNER'] == '1'
    when :rspec
      ENV['RSPEC_RUNNER'] == '1' or Object.const_defined?(:Spec)
    else
      false
    end
  end
end

class Object
  # failure(:rubinius) do
  #   Do NOT run these specs on the listed platform.
  #   Used for specs that are expected to fail, perhaps because of
  #   a bug on the listed engine. This is not for specs that are
  #   noncompliant. This is a convenience to prevent executing
  #   specs on a particular engine, perhaps because executing
  #   the spec causes particularly nasty behavior.
  # end
  def failure(*engines)
    yield unless MSpec.guard?(*engines) { |engine| MSpec.engine? engine }
  end

  # extension(:rubinus, :cardinal) do
  #   Run these specs on the listed engines.
  #   Used for specs that do NOT behave differently than a Ruby
  #   "standard" behavior, perhaps because no equivalent behavior
  #   exists in Ruby.
  # end
  def extension(*engines)
    yield if MSpec.guard?(*engines) { |engine| MSpec.engine? engine }
  end

  # compliant(:ruby, :jruby) do
  #   Run these specs on the listed engines. 
  #   Used for specs that conform to the Ruby "standard"
  # end
  def compliant(*engines)
    yield if MSpec.guard?(*engines) { |engine| MSpec.engine? engine }
  end

  # noncompliant(:rubinius, :jruby) do
  #   Run these specs on the listed engines.
  #   Used for specs that do not conform to the Ruby "standard"
  # end
  def noncompliant(*engines)
    yield if MSpec.guard?(*engines) { |engine| MSpec.engine? engine }
  end

  # version :not, '1.8.4', '1.8.6' do
  #   run these specs if RUBY_VERSION is not 1.8.4 or 1.8.6
  # end
  #
  # version '1.8.4'..'1.8.6' do
  #   run these specs if RUBY_VERSION is in the range 1.8.4 to 1.8.6
  # end
  def version(*args)
    yield if MSpec.guard?(*args) { |a| a === RUBY_VERSION }
  end

  # platform :darwin, :mswin do
  #   run these specs if RUBY_PLATFORM matches :darwin or :mswin
  # end
  #
  # platform :not, :mswin do
  #   run these specs if RUBY_PLATFORM does not match :mswin
  # end
  def platform(*args)
    yield if MSpec.guard?(*args) { |a| RUBY_PLATFORM.match(a.to_s) }
  end
  
  # runner :mspec do
  #   run these specs if MSpec is the spec runner
  # end
  #
  # runner :not :rspec do
  #   run these specs if RSpec is not the spec runner
  # end
  def runner(*args)
    yield if MSpec.guard?(*args) { |r| MSpec.runner? r }
  end
end
