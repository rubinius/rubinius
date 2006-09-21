m = Class.methods
p m.kind_of?(MethodTable)
p m.kind_of?(Array)
p MethodTable === m
p Array === m
