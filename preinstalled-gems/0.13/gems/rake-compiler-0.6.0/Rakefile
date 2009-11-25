#--
# Copyright (c) 2008 Luis Lavena
#
# This source code is released under the MIT License.
# See LICENSE file for details
#++

#
# NOTE: Keep this file clean.
# Add your customizations inside tasks directory.
# Thank You.
#

begin
  require 'rake'
rescue LoadError
  require 'rubygems'
  gem 'rake', '~> 0.8.3.1'
  require 'rake'
end

# load rakefile extensions (tasks)
Dir['tasks/*.rake'].sort.each { |f| load f }
