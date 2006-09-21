@type = Type.MemorySystem

# The constant memory is an instance of memorysystem
add_const :Memory, @type

add(:allocate_memory) do |i|
  i.inline = true
  i.type = Type.Fixnum
  i.args = [Type.Fixnum]
  i.gen do |g, cur, sz|
    sz = args.shift
    t = g.temp(Type.Fixnum)
    t.cast_to "malloc(#{sz})"
  end
end

add(:resize_memory) do |i|
  i.inline = true
  i.type = Type.Fixnum
  i.args = [Type.Fixnum, Type.Fixnum]
  i.gen do |g, cur, args|
    addr = args.shift
    sz = args.shift
    t = g.temp(Type.Fixnum)
    t.cast_to "realloc((void*)#{addr}, #{sz})"
  end
end

add(:release_memory) do |i|
  i.type = Type.nil
  i.args = [Type.Fixnum]
  i.gen do |g, cur, args|
    addr = args.shift
    "free((void*)#{addr})"
  end
end

add(:fetch_long) do |i|
  i.type = Type.Fixnum
  i.args = [Type.Fixnum, Type.Fixnum]
  i.gen do |g, cur, args|
    addr = args.shift
    idx = args.shift
    t = g.temp("long*")
    t2 = g.temp(Type.Fixnum)
    g << "#{t} = (long*)(#{addr} + (#{args.shift} * sizeof(long)))"
    g << t2.cast_to("*#{t}")
    g << "#{t2}"
  end
end

add(:store_long) do |i|
  i.type = Type.Fixnum
  i.args = [Type.Fixnum, Type.Fixnum, Type.Fixnum]
  i.gen do |g, cur, args|
    t = g.temp("long*")
    addr = args.shift
    idx = args.shift
    val = args.shift
    g << "#{t} = (long*)(#{addr} + (#{idx} * sizeof(long)))"
    g << "*#{t} = #{val}"
    g << "#{val}"
  end  
end

add(:fetch_byte) do |i|
  i.type = Type.Fixnum
  i.args = [Type.Fixnum]
  i.gen do |g, cur, args|
     addr = args.shift
     t = g.temp("char*")
     g << t.cast_as(addr);
     g << "(long)(*#{t})"
  end
end

add(:store_byte) do |i|
  i.type = Type.Fixnum
  i.args = [Type.Fixnum, Type.Fixnum]
  i.gen do |g, cur, args|
    addr = args.shift
    val = args.shift
    t2 = g.temp(Type.Fixnum)
    t = g.temp("char*")
    g << t2.cast_as(val)
    g << t.cast_as(addr);
    g << "*#{t} = (char)(#{t2})"
    g << "#{t2}"
  end
end

add(:pointer_size) do |i|
  i.type = Type.Fixnum
  i.gen do |g, s, args|
    "sizeof(void*)"
  end
end

add(:long_size) do |i|
  i.type = Type.Fixnum
  i.gen do |g, s, args|
    "sizeof(long)"
  end
end

add(:transfer_memory) do |i|
  i.type = Type.void
  i.args = [Type.Fixnum, Type.Fixnum, Type.Fixnum]
  i.gen do |g, s, args|
    src = args.shift
    src_size = args.shift
    dest = args.shift
    "memcpy( (void*)#{dest}, (void*)#{src}, (size_t)#{src_size} )"
  end
end

add(:clear_memory) do |i|
  i.type = Type.void
  i.args = [Type.Fixnum, Type.Fixnum]
  i.gen do |g, s, args|
    addr = args.shift
    sz = args.shift
    "memset((void*)#{addr}, 0, (size_t)#{sz})"
  end
end

add(:compare_memory) do |i|
  i.type = Type.Fixnum
  i.args = [Type.Fixnum, Type.Fixnum, Type.Fixnum]
  i.gen do |g, s, args|
    b1 = args.shift
    b2 = args.shift
    size = args.shift
    "memcmp((void*)#{b1}, (void*)#{b2}, (size_t)#{size})"
  end
end