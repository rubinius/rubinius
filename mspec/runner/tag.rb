class SpecTag
  attr_reader :tag, :comment, :description
  
  def initialize(string=nil)
    parse(string) if string
  end
  
  def parse(string)
    m = /^([^()#:]+)(\(([^):]+)?\))?:(.*)/.match string
    @tag, @comment, @description = m.values_at(1, 3, 4) if m
  end
  
  def to_s
    "#{@tag}#{ "(#{@comment})" if @comment }:#{@description}"
  end
end
