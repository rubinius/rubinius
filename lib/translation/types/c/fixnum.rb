@type = Type.Fixnum

def operator(kind, type)
  add(kind) do |i|
    i.type = type
    i.args = [Type.Fixnum]
    i.gen do |g, s, a|
      "#{s} #{kind} #{a}"
    end
  end
end

def operators(type, *a)
  a.each { |i| operator(i, type) }
end

operators Type.bool, :>, :>=, :<, :<=, :==
operators Type.Fixnum, :+, :-, :*, :/, :&, :|

add(:upto) do |i|
  i.type = Type.void
  i.args = [Type.Fixnum]
  i.block = true
  i.yield_args = []
  i.yield_type = Type.void
  i.gen do |g, s, a|
    body = a.pop
    max = a.shift
    cur = g.new_temp(Type.Fixnum)
    g << cur.declare_as(s)
    g << "while(#{cur} <= #{max}) {\n#{body}\n#{cur}++;\n}"
  end
end

add(:downto) do |i|
  i.type = Type.void
  i.args = [Type.Fixnum]
  i.block = true
  i.yield_args = []
  i.yield_type = Type.void
  i.gen do |g, s, a|
    body = a.pop
    max = a.shift
    cur = g.new_temp(Type.Fixnum)
    g << cur.declare_as(s)
    g << "while(#{cur} >= #{max}) {\n#{body}\n#{cur}--;\n}"
  end
end

add(:times) do |i|
  i.type = Type.void
  i.args = []
  i.block = true
  i.yield_args = []
  i.yield_type = Type.void
  i.gen do |g, s, a|
    body = a.pop
    cur = g.new_temp(Type.Fixnum)
    g << cur.declare_as(0)
    g << "while(#{cur} < #{s}) {\n#{body}\n#{cur}++;\n}"
  end
end