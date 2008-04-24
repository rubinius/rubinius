class SpecTag
  attr_accessor :tag, :comment, :description

  def initialize(string=nil)
    parse(string) if string
  end

  def parse(string)
    m = /^([^()#:]+)(\(([^)]+)?\))?:(.*)/.match string
    @tag, @comment, @description = m.values_at(1, 3, 4) if m
  end

  def to_s
    "#{@tag}#{ "(#{@comment})" if @comment }:#{@description}"
  end

  def ==(o)
    @tag == o.tag and @comment == o.comment and @description == o.description
  end
end
