cm = def hello
  puts "hello evan!"
end

def context
  MethodContext.current.dup
end

ctx = context()
p cm

ctx.sender = MethodContext.current
ctx.ip = 0
ctx.block = 0
ctx.method = cm
ctx.literals = cm.literals
ctx.receiver = self
ctx.locals = nil
ctx.argcount = 0
ctx.name = :hello
ctx.method_module = Object

p ctx.to_s

ctx.activate
