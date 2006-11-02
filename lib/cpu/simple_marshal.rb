class SimpleMarshal
  def marshal(obj)
    if obj.fixnum?
      marshal_int obj
    elsif obj.symbol?
      marshal_sym obj
    elsif obj.reference?
      kls = obj.rclass
      if kls == CPU::Global.string
        marshal_str(obj)
      elsif kls == CPU::Global.tuple
        marshal_tup(obj)
      elsif kls == CPU::Global.cmethod
        marshal_cmethod(obj)
      elsif kls == CPU::Global.bytearray
        marshal_bytes(obj)
      else
        kls.as :class
        name = kls.name
        name.as :symbol
        raise "Unable to marshal a reference for a '#{name.string.as_string}' (#{kls.address}, #{CPU::Global.tuple.address})"
      end
    elsif obj.nil?
      ?n.chr
    elsif obj.true?
      ?t.chr
    elsif obj.false?
      ?f.chr
    else
      raise "Unable to marshal a #{obj.class}, #{obj.inspect}"
    end
  end
  
  class MarshalState
    def initialize
      @consumed = 0
    end
    
    attr_accessor :consumed
  end
  
  STDOUT.sync = true
  
  def unmarshal(str, state=nil)
    state ||= MarshalState.new
    tag = str[0]
    start = state.consumed
    # print tag.chr + "#{start}("
    out = case tag
    when ?i
      unmarshal_int(str, state)
    when ?s
      unmarshal_str(str, state)
    when ?x
      unmarshal_sym(str, state)
    when ?p
      unmarshal_tup(str, state)
    when ?b
      unmarshal_bytes(str, state)
    when ?m
      unmarshal_cmethod(str, state)
    when ?n
      state.consumed += 1
      RObject.nil
    when ?t
      state.consumed += 1
      RObject.true
    when ?f
      state.consumed += 1
      RObject.false      
    else
      raise "Unable to unmarshal #{str.inspect}, unknown type #{tag} (#{tag.chr})"
    end
    # print " [#{state.consumed - start}]) "
    out
  end
  
  def marshal_int(int)
    ?i.chr + [int.to_int].pack("i")
  end
    
  def unmarshal_int(str, state)
    state.consumed += 5
    int = str[1..-1].unpack("i").first
    return RObject.wrap(int)
  end
  
  def marshal_str(obj)
    obj.as :string
    str = obj.as_string
    sz = str.size
    ?s.chr + [sz].pack("I") + str
  end
  
  def unmarshal_str(str, state)
    sz = str[1,4].unpack("I").first
    state.consumed += (5 + sz)
    return Rubinius::String.new(str[5,sz])
  end
  
  def marshal_sym(obj)
    obj.as :symbol
    out = marshal_str obj.string
    out[0] = ?x
    return out
  end
  
  def unmarshal_sym(str, state)
    unmarshal_str(str, state).to_sym
  end
  
  def marshal_tup(tup)
    marshal_fields_as tup, ?p
  end
  
  def marshal_fields_as(tup, type)
    sz = tup.fields
    str = type.chr + [sz].pack("I")
    0.upto(sz-1) do |idx|
      str << marshal(tup.at(idx))
    end
    return str
  end
  
  def unmarshal_tup(str, state)
    sz = unmarshal_num_fields str
    tup = Rubinius::Tuple.new(sz)
    unmarshal_into_fields str, state, sz, tup
  end
  
  def unmarshal_num_fields(str)
    sz = str.slice(1,4).unpack("I").first
  end
  
  def unmarshal_into_fields(str, state, sz, tup)
    str = str.dup
    str.slice! 0, 5
    state.consumed += 5
    cur = state.consumed
    0.upto(sz - 1) do |idx|
      # puts "#{idx}: #{str.dump}"
      o = unmarshal(str, state)
      # puts " (slicing #{state.consumed - cur} bytes) "
      str.slice!(0, state.consumed - cur)
      cur = state.consumed
      tup.put idx, o
    end
    
    return tup
  end
  
  def marshal_bytes(obj)
    obj.as :bytearray
    str = obj.as_string
    sz = str.size
    ?b.chr + [sz].pack("I") + str
  end
  
  def unmarshal_bytes(str, state)
    sz = str[1,4].unpack("I").first
    state.consumed += (5 + sz)
    return Rubinius::ByteArray.from_string(str[5,sz])
  end
  
  def marshal_cmethod(cm)
    marshal_fields_as cm, ?m
  end
  
  def unmarshal_cmethod(str, state)
    sz = unmarshal_num_fields str
    obj = Rubinius::CompiledMethod.allocate
    unmarshal_into_fields str, state, sz, obj
    return obj
  end
end
