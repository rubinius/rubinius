r, w = IO.pipe

w.write "hello"
puts r.read(5)
