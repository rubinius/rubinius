require File.dirname(__FILE__) + '/../../expectations'
require File.dirname(__FILE__) + '/base'

class CIFormatter < BaseFormatter
  def print_failure(i,r)
    @out.print r.describe + " " + r.it + "\n"
  end
  
  def summary
    unless @summarized
      @exceptions.each_with_index do |r,i|
        print_failure(i+1,r)
      end
      @summarized = true
    end
  end
end
