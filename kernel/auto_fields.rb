class MethodTable
def instance_variables; Ruby.asm "push self\npush 0\nfetch_field"; end
def keys; Ruby.asm "push self\npush 1\nfetch_field"; end
def values; Ruby.asm "push self\npush 2\nfetch_field"; end
def bins; Ruby.asm "push self\npush 3\nfetch_field"; end
def entries; Ruby.asm "push self\npush 4\nfetch_field"; end
def default; Ruby.asm "push self\npush 5\nfetch_field"; end
end
class Class
def instance_variables; Ruby.asm "push self\npush 0\nfetch_field"; end
def methods; Ruby.asm "push self\npush 1\nfetch_field"; end
def method_cache; Ruby.asm "push self\npush 2\nfetch_field"; end
def name; Ruby.asm "push self\npush 3\nfetch_field"; end
def constants; Ruby.asm "push self\npush 4\nfetch_field"; end
def superclass; Ruby.asm "push self\npush 5\nfetch_field"; end
def instance_fields; Ruby.asm "push self\npush 6\nfetch_field"; end
def instance_flags; Ruby.asm "push self\npush 7\nfetch_field"; end
end
class MatchData
def instance_variables; Ruby.asm "push self\npush 0\nfetch_field"; end
def source; Ruby.asm "push self\npush 1\nfetch_field"; end
def regexp; Ruby.asm "push self\npush 2\nfetch_field"; end
def full; Ruby.asm "push self\npush 3\nfetch_field"; end
def region; Ruby.asm "push self\npush 4\nfetch_field"; end
end
class MetaClass
def instance_variables; Ruby.asm "push self\npush 0\nfetch_field"; end
def methods; Ruby.asm "push self\npush 1\nfetch_field"; end
def method_cache; Ruby.asm "push self\npush 2\nfetch_field"; end
def name; Ruby.asm "push self\npush 3\nfetch_field"; end
def constants; Ruby.asm "push self\npush 4\nfetch_field"; end
def superclass; Ruby.asm "push self\npush 5\nfetch_field"; end
def instance_fields; Ruby.asm "push self\npush 6\nfetch_field"; end
def instance_flags; Ruby.asm "push self\npush 7\nfetch_field"; end
def attached_instance; Ruby.asm "push self\npush 8\nfetch_field"; end
end
class Regexp
def instance_variables; Ruby.asm "push self\npush 0\nfetch_field"; end
def source; Ruby.asm "push self\npush 1\nfetch_field"; end
def data; Ruby.asm "push self\npush 2\nfetch_field"; end
def names; Ruby.asm "push self\npush 3\nfetch_field"; end
end
class CompiledMethod
def instance_variables; Ruby.asm "push self\npush 0\nfetch_field"; end
def primitive; Ruby.asm "push self\npush 1\nfetch_field"; end
def required; Ruby.asm "push self\npush 2\nfetch_field"; end
def bytecodes; Ruby.asm "push self\npush 3\nfetch_field"; end
def locals; Ruby.asm "push self\npush 4\nfetch_field"; end
def literals; Ruby.asm "push self\npush 5\nfetch_field"; end
def arguments; Ruby.asm "push self\npush 6\nfetch_field"; end
def scope; Ruby.asm "push self\npush 7\nfetch_field"; end
def exceptions; Ruby.asm "push self\npush 8\nfetch_field"; end
def lines; Ruby.asm "push self\npush 9\nfetch_field"; end
def file; Ruby.asm "push self\npush 10\nfetch_field"; end
def name; Ruby.asm "push self\npush 11\nfetch_field"; end
end
class SymbolTable
def instance_variables; Ruby.asm "push self\npush 0\nfetch_field"; end
def symbols; Ruby.asm "push self\npush 1\nfetch_field"; end
def strings; Ruby.asm "push self\npush 2\nfetch_field"; end
end
class BlockEnvironment
def instance_variables; Ruby.asm "push self\npush 0\nfetch_field"; end
def home; Ruby.asm "push self\npush 1\nfetch_field"; end
def initial_ip; Ruby.asm "push self\npush 2\nfetch_field"; end
def last_ip; Ruby.asm "push self\npush 3\nfetch_field"; end
def post_send; Ruby.asm "push self\npush 4\nfetch_field"; end
end
class Array
def total; Ruby.asm "push self\npush 0\nfetch_field"; end
def tuple; Ruby.asm "push self\npush 1\nfetch_field"; end
end
class MethodContext
def instance_variables; Ruby.asm "push self\npush 0\nfetch_field"; end
def sender; Ruby.asm "push self\npush 1\nfetch_field"; end
def ip; Ruby.asm "push self\npush 2\nfetch_field"; end
def sp; Ruby.asm "push self\npush 3\nfetch_field"; end
def block; Ruby.asm "push self\npush 4\nfetch_field"; end
def raiseable; Ruby.asm "push self\npush 5\nfetch_field"; end
def method; Ruby.asm "push self\npush 6\nfetch_field"; end
def bytecodes; Ruby.asm "push self\npush 7\nfetch_field"; end
def literals; Ruby.asm "push self\npush 8\nfetch_field"; end
def receiver; Ruby.asm "push self\npush 9\nfetch_field"; end
def locals; Ruby.asm "push self\npush 10\nfetch_field"; end
def argcount; Ruby.asm "push self\npush 11\nfetch_field"; end
def name; Ruby.asm "push self\npush 12\nfetch_field"; end
def module; Ruby.asm "push self\npush 13\nfetch_field"; end
end
class Hash
def instance_variables; Ruby.asm "push self\npush 0\nfetch_field"; end
def keys; Ruby.asm "push self\npush 1\nfetch_field"; end
def values; Ruby.asm "push self\npush 2\nfetch_field"; end
def bins; Ruby.asm "push self\npush 3\nfetch_field"; end
def entries; Ruby.asm "push self\npush 4\nfetch_field"; end
def default; Ruby.asm "push self\npush 5\nfetch_field"; end
end
class String
def bytes; Ruby.asm "push self\npush 0\nfetch_field"; end
def characters; Ruby.asm "push self\npush 1\nfetch_field"; end
def encoding; Ruby.asm "push self\npush 2\nfetch_field"; end
def data; Ruby.asm "push self\npush 3\nfetch_field"; end
end
class IO
def descriptor; Ruby.asm "push self\npush 0\nfetch_field"; end
end
class Module
def instance_variables; Ruby.asm "push self\npush 0\nfetch_field"; end
def methods; Ruby.asm "push self\npush 1\nfetch_field"; end
def method_cache; Ruby.asm "push self\npush 2\nfetch_field"; end
def name; Ruby.asm "push self\npush 3\nfetch_field"; end
def constants; Ruby.asm "push self\npush 4\nfetch_field"; end
end
class BlockContext
def instance_variables; Ruby.asm "push self\npush 0\nfetch_field"; end
def sender; Ruby.asm "push self\npush 1\nfetch_field"; end
def ip; Ruby.asm "push self\npush 2\nfetch_field"; end
def sp; Ruby.asm "push self\npush 3\nfetch_field"; end
def block; Ruby.asm "push self\npush 4\nfetch_field"; end
def raiseable; Ruby.asm "push self\npush 5\nfetch_field"; end
def env; Ruby.asm "push self\npush 6\nfetch_field"; end
end
class Object
def instance_variables; Ruby.asm "push self\npush 0\nfetch_field"; end
end
