require 'mspec/runner/filters/match'

class ActionFilter
  def initialize(tag, desc)
    all =  Array(desc)
    all += MSpec.read_tags(*Array(tag)).map { |t| t.description }
    @filter = MatchFilter.new(nil, *all) unless all.empty?
  end
  
  def ===(string)
    @filter === string if @filter
  end
end
