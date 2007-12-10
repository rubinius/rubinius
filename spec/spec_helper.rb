$:.push File.dirname(__FILE__) + '/../'

if ENV['MSPEC_RUNNER'] == '1'
  require 'mspec/complete'
  require 'mspec/mocks'
else
  require 'mspec/runner/guards'
  require 'mspec/runner/shared'
  require 'mspec/matchers/be_ancestor_of'
  require 'mspec/matchers/output'

  TOLERANCE = 0.00003 unless Object.const_defined?(:TOLERANCE)
end

$verbose = $VERBOSE
$VERBOSE = nil

at_exit { $VERBOSE = $verbose }
