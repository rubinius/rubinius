$:.push File.dirname(__FILE__) + '/../'

if ENV['MSPEC_RUNNER'] == '1'
  require 'mspec/complete'
  require 'mspec/mocks'
  require 'pp'
else
  require 'mspec/runner/guards'
  require 'mspec/runner/shared'
  require 'mspec/matchers/be_ancestor_of'
  require 'mspec/matchers/output'
  require 'mspec/matchers/output_to_fd'
  require 'mspec/matchers/complain'

  TOLERANCE = 0.00003 unless Object.const_defined?(:TOLERANCE)
end

unless Object.const_defined?(:OUTPUT_WARNINGS)
  $verbose = $VERBOSE
  $VERBOSE = nil

  at_exit { $VERBOSE = $verbose }
end

def bignum_value(plus=0)
  0x8000_0000_0000_0000 + plus
end
