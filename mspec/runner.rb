require 'mspec/mocks'
require 'mspec/runner/runner'
require 'mspec/runner/object'
require 'mspec/runner/formatters'
require 'mspec/runner/guards'
require 'mspec/runner/shared'

def $stderr.write(str)
  # The useless use warnings are a crime against OO.
  str =~ /useless use of/ ? nil : super
end
