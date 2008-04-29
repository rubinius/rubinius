dir_path = File.dirname(__FILE__)
lib_path = File.expand_path(dir_path + '/../lib')
bin_path = File.expand_path(dir_path + '/..')
$:.unshift lib_path unless $:.include? lib_path
$:.unshift bin_path unless $:.include? bin_path

require 'mspec/helpers/io'
require 'mspec/helpers/scratch'

# Remove this when MRI has intelligent warnings
$VERBOSE = nil unless $VERBOSE

module Kernel
  def pretty_inspect
    inspect
  end
end
