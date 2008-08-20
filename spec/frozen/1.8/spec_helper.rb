unless ENV['MSPEC_RUNNER']
  begin
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

    TOLERANCE = 0.00003 unless Object.const_defined?(:TOLERANCE)
  rescue LoadError
    puts "Please install the MSpec gem to run the specs."
    exit 1
  end
end

v = MSpec::VERSION.split('.').collect { |d| "1%02d" % d.to_i }.join.to_i
unless v >= 101104100
  puts "Please install MSpec version >= 1.4.0 to run the specs"
  exit 1
end

$VERBOSE = nil unless ENV['OUTPUT_WARNINGS']

def has_tty?
  if STDOUT.tty? then
    yield
  end
end
