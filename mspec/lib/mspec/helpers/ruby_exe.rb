require 'mspec/ruby_name'

# The ruby_exe helper provides a wrapper for invoking the
# same Ruby interpreter as the one running the specs and
# getting the output from running the code. If +code+ is a
# file that exists, it will be run. Otherwise, +code+ should
# be Ruby code that will be run with the -e command line
# option. For example:
#
#   ruby_exe('path/to/some/file.rb')
#
# will be executed as
#
#   `#{RUBY_EXE} #{'path/to/some/file.rb'}`
#
# while
#
#   ruby_exe('puts "hello, world."')
#
# will be executed as
#
#   `#{RUBY_EXE} -e #{'puts "hello, world."'}`
#
# The RUBY_EXE constant can be set explicitly since the value
# is used each time ruby_exe is invoked. The mspec runner script
# will set ENV['RUBY_EXE'] to the name of the executable used
# to invoke the mspec-run script. The value of RUBY_EXE will be
# constructed as follows:
#
#   1. the value of ENV['RUBY_EXE']
#   2. an explicit value based on RUBY_NAME
#   3. cwd/(RUBY_NAME + $(EXEEXT) || $(exeext) || '')
#   4. $(bindir)/$(RUBY_INSTALL_NAME)
#
# The value will only be used if the file exists and is executable.
#
# These 4 ways correspond to the following scenarios:
#
#   1. Using the MSpec runner scripts, the name of the
#      executable is explicitly passed by ENV['RUBY_EXE']
#      so there is no ambiguity.
#
#  Otherwise, if using RSpec (or something else)
#
#   2. Running the specs while developing an alternative
#      Ruby implementation. This explicitly names the
#      executable in the development directory based on
#      the value of RUBY_NAME, which is probably initialized
#      from the value of RUBY_ENGINE.
#   3. Running the specs within the source directory for
#      some implementation. (E.g. a local build directory.)
#   4. Running the specs against some installed Ruby
#      implementation.

class Object
  def ruby_exe_options(option)
    case option
    when :env
      ENV['RUBY_EXE']
    when :engine
      case RUBY_NAME
      when 'rbx'
        "bin/rbx"
      when 'jruby'
        "bin/jruby"
      when 'ironruby'
        "ir"
      end
    when :name
      bin = RUBY_NAME + (Config::CONFIG['EXEEXT'] || Config::CONFIG['exeext'] || '')
      File.join(".", bin)
    when :install_name
      bin = Config::CONFIG["RUBY_INSTALL_NAME"] || Config::CONFIG["ruby_install_name"]
      bin << (Config::CONFIG['EXEEXT'] || Config::CONFIG['exeext'] || '')
      File.join(Config::CONFIG['bindir'], bin)
    end
  end

  def resolve_ruby_exe
    [:env, :engine, :name, :install_name].each do |option|
      exe = ruby_exe_options option
      return exe if exe and File.exists?(exe) and File.executable?(exe)
    end
    nil
  end

  def ruby_exe(code)
    if File.exists?(code) and File.executable?(code)
      `#{RUBY_EXE} #{ENV['RUBY_FLAGS']} #{code}`
    else
      `#{RUBY_EXE} #{ENV['RUBY_FLAGS']} -e #{code.inspect}`
    end
  end

  unless Object.const_defined?(:RUBY_EXE) and RUBY_EXE
    require 'rbconfig'

    RUBY_EXE = resolve_ruby_exe or
      raise Exception, "Unable to find a suitable ruby executable."
  end
end
