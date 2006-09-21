@type = Type.RObject

add(:put) do |i|
  i.type = Type.RObject
  i.args = [Type.Fixnum, Type.RObject]
  i.gen do |g, me, args|
    idx = args.shift
    obj = args.shift
    
    "SET_FIELD(#{me}, #{idx}, #{obj})"
  end
end

add(:at) do |i|
  i.type = Type.RObject
  i.args = [Type.Fixnum]
  i.gen do |g, me, args|
    idx = args.shift
    
    "NTH_FIELD(#{me}, #{idx})"
  end
end

add(:address) do |i|
  i.type = Type.Fixnum
  i.args = []
  i.gen do |g, me, args|
    "(OBJECT)(me)"
  end
end

def immediate(name, val)
  add(name, Type.RObjectMeta) do |i|
    i.type = Type.RObject
    i.args = []
    i.gen do |g, me, args|
      "(OBJECT)(#{val})"
    end
  end
end

immediate :undef, 6
immediate :nil, 4
immediate :true, 2
immediate :false, 0

# Turns RObject.new into a noop, just casting the argment (a fixnum)
# into an OBJECT.
add(:new, Type.RObjectMeta) do |i|
  i.type = Type.RObject
  i.args = [Type.Fixnum]
  i.gen do |g, me, args|
    "(OBJECT)(#{args.shift})"
  end
end
