unless ENV['MSPEC_RUNNER']
  begin
    require 'mspec/helpers'
    require 'mspec/guards'
    require 'mspec/runner/shared'
    require 'mspec/matchers/be_ancestor_of'
    require 'mspec/matchers/output'
    require 'mspec/matchers/output_to_fd'
    require 'mspec/matchers/complain'

    TOLERANCE = 0.00003 unless Object.const_defined?(:TOLERANCE)
  rescue LoadError
    puts "Please install the MSpec gem to run the specs."
    exit 1
  end
end

unless ENV['OUTPUT_WARNINGS']
  $verbose = $VERBOSE
  $VERBOSE = nil

  at_exit { $VERBOSE = $verbose }
end
