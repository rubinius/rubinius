cm = Kernel.methods[:puts]
cm.activate(Class, ["hello."])

Class.__send__(:puts, "hello 2.")


mo = Class.method(:puts)

p mo
mo.call("hello 3!")
