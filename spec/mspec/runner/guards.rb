if defined?(RUBY_ENGINE)
  RUBY_NAME = RUBY_ENGINE
else
  require 'rbconfig'
  RUBY_NAME = Config::CONFIG["RUBY_INSTALL_NAME"]
end

module MSpec
  def self.engine?(name)
    case name
    when :rbx, :rubinius
      RUBY_NAME == 'rbx'
    when :mri, :ruby
      RUBY_NAME =~ /^ruby/
    when :jruby
      RUBY_NAME == 'jruby'
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
end

class Object
  def failure(*engines)
    yield unless MSpec.guard?(*engines) { |engine| MSpec.engine? engine }
  end

  def extension(*engines)
    yield if MSpec.guard?(*engines) { |engine| MSpec.engine? engine }
  end

  def compliant(*engines)
    yield if MSpec.guard?(*engines) { |engine| MSpec.engine? engine }
  end

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
end
