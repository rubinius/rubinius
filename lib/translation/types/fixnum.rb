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