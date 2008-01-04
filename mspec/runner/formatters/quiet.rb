require 'mspec/expectations'
require 'mspec/runner/formatters/base'

class QuietFormatter < BaseFormatter
  def after_it(msg)
  end
end
