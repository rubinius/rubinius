$:.push File.dirname(__FILE__) + '/../'

if ENV['USE_MSPEC'] == '1'
  require  'mspec/complete'
  require 'mspec/mock/mock'
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
