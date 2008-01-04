require 'mspec/runner/formatters/base'

class CIFormatter < BaseFormatter
  def after_it(msg)
    if @current.exception
      @out.print @current.describe + " " + @current.it + "\n"
    end
  end
  
  def print_start() end
end
