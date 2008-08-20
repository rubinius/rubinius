##
# Parent class of everything that can live in a MethodTable.
#
# When calling an Executable, first the primitive slot is checked and executed
# if it exists. If execution of the primitive is successful, the value is put
# on the stack, and execution of the method is finished.  If unsuccessful or
# absent (only in the case of a CompiledMethod) a MethodContext is created and
# activated (see MethodContext).

class Executable
end

