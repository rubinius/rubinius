require 'mspec/runner/mspec'
require 'mspec/runner/actions/tally'

class SpecGuard
  def self.register
    unless @registered
      @tally = TallyAction.new
      @tally.register
      MSpec.register :finish, self
      @registered = true
    end
  end

  def self.unregister
    @tally.unregister if @tally
  end

  def self.finish
    print "\n#{self.class}\n#{@tally.format}\n"
  end

  # Returns a partial Ruby version string based on +which+. For example,
  # if RUBY_VERSION = 8.2.3 and RUBY_PATCHLEVEL = 71:
  #
  #  :major  => "8"
  #  :minor  => "8.2"
  #  :tiny   => "8.2.3"
  #  :teeny  => "8.2.3"
  #  :full   => "8.2.3.71"
  def self.ruby_version(which = :minor)
    case which
    when :major
      n = 1
    when :minor
      n = 2
    when :tiny, :teeny
      n = 3
    else
      n = 4
    end

    patch = RUBY_PATCHLEVEL.to_i
    patch = 0 if patch < 0
    version = "#{RUBY_VERSION}.#{patch}"
    version.split('.')[0,n].join('.')
  end

  def self.windows?(key = RUBY_PLATFORM)
    !!key.match(/(mswin|mingw)/)
  end


  def initialize(*args)
    @args = args
  end

  def yield?(invert=false)
    if MSpec.mode? :unguarded
      return true
    elsif MSpec.mode? :report
      self.class.register
      MSpec.register :before, self
      return true
    elsif MSpec.mode? :verify
      self.class.register
      MSpec.register :after, self
      return true
    end
    return match? ^ invert
  end

  def ===(other)
    true
  end

  def before(state)
  end

  def after(state)
  end

  def unregister
    MSpec.unregister :before, self
    MSpec.unregister :after, self
    MSpec.unregister :exclude, self
    self.class.unregister
  end

  def implementation?(*args)
    args.any? do |name|
      !!case name
      when :rubinius
        RUBY_NAME =~ /^rbx/
      when :ruby
        RUBY_NAME =~ /^ruby/
      when :jruby
        RUBY_NAME =~ /^jruby/
      when :ironruby
        RUBY_NAME =~ /^ironruby/
      when :macruby
        RUBY_NAME =~ /^macruby/
      else
        false
      end
    end
  end

  def standard?
    implementation? :ruby
  end

  def windows?(sym, key)
    sym == :windows && SpecGuard.windows?(key)
  end

  def platform?(*args)
    args.any? do |platform|
      if platform != :java && RUBY_PLATFORM.match('java') && os?(platform)
        true
      else
        RUBY_PLATFORM.match(platform.to_s) || windows?(platform, RUBY_PLATFORM)
      end
    end
  end

  def wordsize?(size)
    size == 8 * 1.size
  end

  def os?(*oses)
    require 'rbconfig'
    oses.any? do |os|
      host_os = Config::CONFIG['host_os'] || RUBY_PLATFORM
      host_os.downcase!
      host_os.match(os.to_s) || windows?(os, host_os)
    end
  end

  def match?
    implementation?(*@args) or platform?(*@args)
  end
end
