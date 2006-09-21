h = { 1 => 3, 8 => :a }
puts h
p h.keys
p h.values

sym = :new
p sym.hash

p Class.methods.get_by_hash(sym.hash, sym)
