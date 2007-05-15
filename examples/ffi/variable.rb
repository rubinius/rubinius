pr = FFI.create_function nil, "printf", [:string, :int], :int
p pr
pr.call("%d rocks\n", 47)

pf = NativeFunction::Variable.new nil, "printf", [:string], :int
p pf
pf.call [:int], "%d really rocks\n", 42
pf[:int].call("%d really rocks\n", 42)
