$:.push File.dirname(__FILE__) + '/../'

# Prefer included mspec source over a gem version
mspec_dir = File.dirname(__FILE__) + '/../../../mspec/lib'
if File.exist? mspec_dir
  $:.unshift mspec_dir
else
  require 'rubygems'
end

if ENV['MSPEC_RUNNER'] == '1'
  require 'mspec'
  require 'pp'
else
  require 'mspec/helpers'
  require 'mspec/guards'
  require 'mspec/runner/shared'
  require 'mspec/matchers/be_ancestor_of'
  require 'mspec/matchers/output'
  require 'mspec/matchers/output_to_fd'
  require 'mspec/matchers/complain'

  TOLERANCE = 0.00003 unless Object.const_defined?(:TOLERANCE)
end

unless ENV['OUTPUT_WARNINGS'] == '1'
  $verbose = $VERBOSE
  $VERBOSE = nil

  at_exit { $VERBOSE = $verbose }
end
