$:.push File.dirname(__FILE__) + '/../'

if ENV['USE_MSPEC'] == '1'
  require  'mspec/complete'
else
  require 'mspec/runner/guards'
  require 'mspec/runner/shared'
  require 'mspec/matchers/be_ancestor_of'
  require 'mspec/matchers/output'
end

require 'mspec/mock/mock'

$verbose = $VERBOSE
$VERBOSE = nil

at_exit { $VERBOSE = $verbose }
