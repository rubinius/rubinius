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
      RUBY_NAME =~ /^ruby/
    when :ruby18
      RUBY_NAME =~ /^ruby(1.8)?/ and RUBY_VERSION =~ /^1.8/
    when :ruby19
      RUBY_NAME =~ /^ruby(1.9)?/ and RUBY_VERSION =~ /^1.9/
    when :jruby
      RUBY_NAME =~ /^jruby/
    else
      false
    end
  end
  
  def self.guard?(*args, &cond)
    unless args.empty?
      result = args.any?(&cond)
    else
      result = yield
    end
    return result
  end
  
  def self.platform?(*platforms)
    MSpec.guard?(*platforms) { |platform| RUBY_PLATFORM.match(platform.to_s) }
  end
  
  def self.version?(*versions)
    MSpec.guard?(*versions) { |version| version === RUBY_VERSION }
  end

  def self.os?(*oses)
      require 'rbconfig'
      MSpec.guard?(*oses) do |os|
        host_os = Config::CONFIG['host_os']  || RUBY_PLATFORM
        host_os.downcase!
        host_os.match(os.to_s)
      end
  end

  def self.date?(*dates)
    MSpec.guard?(*dates) do |date|
      date = case date
      when Range
        first = date.first
        last = date.last
        Time.parse(first)..Time.parse(last)
      else
        Time.parse(date)
      end 
      date === Time.parse(RUBY_RELEASE_DATE)
    end
  end
  
  def self.patch?(*patches)
    MSpec.guard?(*patches) { |patch| patch === RUBY_PATCHLEVEL }
  end
  
  def self.size?(size)
    size == 8 * 1.size
  end
  
  def self.platform_match?(*args)
    options, platforms = args.partition { |a| a.is_a?(Hash) }
    options = options.first

    should_yield = false
    should_yield |= MSpec.platform?(*platforms) unless platforms.empty?
    if options
      should_yield |= MSpec.version?(*options[:version]) if options.key?(:version)
      should_yield |= MSpec.date?(*options[:date]) if options.key?(:date)
      should_yield |= MSpec.patch?(*options[:patch]) if options.key?(:patch)
      should_yield |= MSpec.size?(options[:size]) if options.key?(:size)
      should_yield |= MSpec.os?(*options[:os]) if options.key?(:os)
    end
    should_yield
  end
  
  def self.runner?(*runners)
    MSpec.guard?(*runners) do |runner|
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
end

class Object
  # fails_on(:rubinius) do
  #   Do NOT run these specs on the listed platform.
  #   Used for specs that are expected to fail, perhaps because of
  #   a bug on the listed engine. This is not for specs that are
  #   noncompliant. This is a convenience to prevent executing
  #   specs on a particular engine, perhaps because executing
  #   the spec causes particularly nasty behavior.
  # end
  def fails_on(*engines)
    yield unless MSpec.guard?(*engines) { |engine| MSpec.engine? engine }
  end

  # extended_on(:rubinus, :cardinal) do
  #   Run these specs on the listed engines.
  #   Used for specs that do NOT behave differently than a Ruby
  #   "standard" behavior, perhaps because no equivalent behavior
  #   exists in Ruby.
  # end
  def extended_on(*engines)
    yield if MSpec.guard?(*engines) { |engine| MSpec.engine? engine }
  end

  # compliant_on(:ruby, :jruby) do
  #   Run these specs on the listed engines. 
  #   Used for specs that conform to the Ruby "standard"
  # end
  def compliant_on(*engines)
    yield if MSpec.guard?(*engines) { |engine| MSpec.engine? engine }
  end

  # not_compliant_on(:jruby) do
  #   Run these specs only if the current engine is not specified.
  #   Used for specs known to be permanently failing, broken, or unsupported
  #   by a given implementation or implementations.
  # end
  def not_compliant_on(*engines)
    yield unless MSpec.guard?(*engines) { |engine| MSpec.engine? engine }
  end

  # deviates_on(:rubinius, :jruby) do
  #   Run these specs on the listed engines.
  #   Used for specs that intentionally do not conform to the Ruby "standard"
  # end
  def deviates_on(*engines)
    yield if MSpec.guard?(*engines) { |engine| MSpec.engine? engine }
  end
  
  # platform_is :darwin, :mswin do
  #   run these specs if RUBY_PLATFORM matches :darwin or :mswin
  # end
  #
  # Specify a single version, a version range, or multiple versions
  # platform_is :version => '1.8.4' ...
  # platform_is :version => '1.8.4'..'1.8.6' ...
  # platform_is :version => ['1.8', '1.9']
  #
  # Specify a date or date range that is understood by Time.parse
  # platform_is :date => '2007-09-04' ...
  # platform_is :date => '2007-09-04'..'2007-10-18'
  # platform_is :date => ['2007-10-04', '2007-10-18']
  #
  # Specify a patch level or range
  # platform :patch => '111'
  # platform :patch => '34'..'111'
  # platform :patch => ['34', '111']
  #
  # Specify a host OS
  # platform :os => [:linux, :mswin]
  def platform_is(*args)
    yield if MSpec.platform_match?(*args)
  end

  # platform_is_not :mswin do
  #   run these specs if RUBY_PLATFORM does not match :mswin
  # end
  def platform_is_not(*args)
    yield unless MSpec.platform_match?(*args)
  end
  
  # runner_is :mspec do
  #   run these specs if MSpec is the spec runner
  # end
  def runner_is(*args)
    yield if MSpec.runner?(*args)
  end

  # runner_is_not :rspec do
  #   run these specs if RSpec is not the spec runner
  # end
  def runner_is_not(*args)
    yield unless MSpec.runner?(*args)
  end
  
  # quarantine! do
  #   do NOT run these specs. Used for specs that are questionable
  #   for whatever reason to removed them from execution while they
  #   are investigated.
  # end
  def quarantine!
  end
end
