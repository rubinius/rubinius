unless ENV['MSPEC_RUNNER']
  begin
    require "pp"
    require 'mspec/version'
    require 'mspec/helpers'
    require 'mspec/guards'
    require 'mspec/runner/shared'
    require 'mspec/matchers/be_ancestor_of'
    require 'mspec/matchers/output'
    require 'mspec/matchers/output_to_fd'
    require 'mspec/matchers/complain'
    require 'mspec/matchers/equal_element'
    require 'mspec/matchers/equal_utf16'
    require 'mspec/matchers/match_yaml'

    # Code to setup HOME directory correctly on Windows
    # This duplicates Ruby 1.9 semantics for defining HOME
    platform_is :windows do
      if ENV['HOME']
        ENV['HOME'] = ENV['HOME'].tr '\\', '/'
      elsif ENV['HOMEDIR'] && ENV['HOMEDRIVE']
        ENV['HOME'] = File.join(ENV['HOMEDRIVE'], ENV['HOMEDIR'])
      elsif ENV['HOMEDIR']
        ENV['HOME'] = ENV['HOMEDIR']
      elsif ENV['HOMEDRIVE']
        ENV['HOME'] = ENV['HOMEDRIVE']
      elsif ENV['USERPROFILE']
        ENV['HOME'] = ENV['USERPROFILE']
      else
        puts "No suitable HOME environment found. This means that all of HOME, HOMEDIR, HOMEDRIVE, and USERPROFILE are not set"
        exit 1
      end
    end

    TOLERANCE = 0.00003 unless Object.const_defined?(:TOLERANCE)
  rescue LoadError
    puts "Please install the MSpec gem to run the specs."
    exit 1
  end
end

v = MSpec::VERSION.split('.').collect { |d| "1%02d" % d.to_i }.join.to_i
unless v >= 101105101
  puts "Please install MSpec version >= 1.5.1 to run the specs"
  exit 1
end

$VERBOSE = nil unless ENV['OUTPUT_WARNINGS']

def has_tty?
  if STDOUT.tty? then
    yield
  end
end
