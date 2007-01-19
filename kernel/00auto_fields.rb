class CompiledMethod
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
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
  def path; Ruby.asm "push self\npush 12\nfetch_field"; end
  ivar_as_index :__ivars__ => 0, :primitive => 1, :required => 2, :bytecodes => 3, :locals => 4, :literals => 5, :arguments => 6, :scope => 7, :exceptions => 8, :lines => 9, :file => 10, :name => 11, :path => 12
end
class SymbolTable
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  def symbols; Ruby.asm "push self\npush 1\nfetch_field"; end
  def strings; Ruby.asm "push self\npush 2\nfetch_field"; end
  ivar_as_index :__ivars__ => 0, :symbols => 1, :strings => 2
end
class MetaClass
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  def methods; Ruby.asm "push self\npush 1\nfetch_field"; end
  def method_cache; Ruby.asm "push self\npush 2\nfetch_field"; end
  def name; Ruby.asm "push self\npush 3\nfetch_field"; end
  def constants; Ruby.asm "push self\npush 4\nfetch_field"; end
  def parent; Ruby.asm "push self\npush 5\nfetch_field"; end
  def superclass; Ruby.asm "push self\npush 6\nfetch_field"; end
  def instance_fields; Ruby.asm "push self\npush 7\nfetch_field"; end
  def instance_flags; Ruby.asm "push self\npush 8\nfetch_field"; end
  def attached_instance; Ruby.asm "push self\npush 9\nfetch_field"; end
  ivar_as_index :__ivars__ => 0, :methods => 1, :method_cache => 2, :name => 3, :constants => 4, :parent => 5, :superclass => 6, :instance_fields => 7, :instance_flags => 8, :attached_instance => 9
end
class Class
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  def methods; Ruby.asm "push self\npush 1\nfetch_field"; end
  def method_cache; Ruby.asm "push self\npush 2\nfetch_field"; end
  def name; Ruby.asm "push self\npush 3\nfetch_field"; end
  def constants; Ruby.asm "push self\npush 4\nfetch_field"; end
  def parent; Ruby.asm "push self\npush 5\nfetch_field"; end
  def superclass; Ruby.asm "push self\npush 6\nfetch_field"; end
  def instance_fields; Ruby.asm "push self\npush 7\nfetch_field"; end
  def instance_flags; Ruby.asm "push self\npush 8\nfetch_field"; end
  ivar_as_index :__ivars__ => 0, :methods => 1, :method_cache => 2, :name => 3, :constants => 4, :parent => 5, :superclass => 6, :instance_fields => 7, :instance_flags => 8
end
class BlockEnvironment
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  def home; Ruby.asm "push self\npush 1\nfetch_field"; end
  def initial_ip; Ruby.asm "push self\npush 2\nfetch_field"; end
  def last_ip; Ruby.asm "push self\npush 3\nfetch_field"; end
  def post_send; Ruby.asm "push self\npush 4\nfetch_field"; end
  ivar_as_index :__ivars__ => 0, :home => 1, :initial_ip => 2, :last_ip => 3, :post_send => 4
end
class MethodContext
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
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
  ivar_as_index :__ivars__ => 0, :sender => 1, :ip => 2, :sp => 3, :block => 4, :raiseable => 5, :method => 6, :bytecodes => 7, :literals => 8, :receiver => 9, :locals => 10, :argcount => 11, :name => 12, :module => 13
end
class Array
  def total; Ruby.asm "push self\npush 0\nfetch_field"; end
  def tuple; Ruby.asm "push self\npush 1\nfetch_field"; end
  ivar_as_index :total => 0, :tuple => 1
end
class Hash
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  def keys; Ruby.asm "push self\npush 1\nfetch_field"; end
  def values; Ruby.asm "push self\npush 2\nfetch_field"; end
  def bins; Ruby.asm "push self\npush 3\nfetch_field"; end
  def entries; Ruby.asm "push self\npush 4\nfetch_field"; end
  def default; Ruby.asm "push self\npush 5\nfetch_field"; end
  ivar_as_index :__ivars__ => 0, :keys => 1, :values => 2, :bins => 3, :entries => 4, :default => 5
end
class Module
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  def methods; Ruby.asm "push self\npush 1\nfetch_field"; end
  def method_cache; Ruby.asm "push self\npush 2\nfetch_field"; end
  def name; Ruby.asm "push self\npush 3\nfetch_field"; end
  def constants; Ruby.asm "push self\npush 4\nfetch_field"; end
  def parent; Ruby.asm "push self\npush 5\nfetch_field"; end
  ivar_as_index :__ivars__ => 0, :methods => 1, :method_cache => 2, :name => 3, :constants => 4, :parent => 5
end
class String
  def bytes; Ruby.asm "push self\npush 0\nfetch_field"; end
  def characters; Ruby.asm "push self\npush 1\nfetch_field"; end
  def encoding; Ruby.asm "push self\npush 2\nfetch_field"; end
  def data; Ruby.asm "push self\npush 3\nfetch_field"; end
  ivar_as_index :bytes => 0, :characters => 1, :encoding => 2, :data => 3
end
class BlockContext
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  def sender; Ruby.asm "push self\npush 1\nfetch_field"; end
  def ip; Ruby.asm "push self\npush 2\nfetch_field"; end
  def sp; Ruby.asm "push self\npush 3\nfetch_field"; end
  def block; Ruby.asm "push self\npush 4\nfetch_field"; end
  def raiseable; Ruby.asm "push self\npush 5\nfetch_field"; end
  def env; Ruby.asm "push self\npush 6\nfetch_field"; end
  ivar_as_index :__ivars__ => 0, :sender => 1, :ip => 2, :sp => 3, :block => 4, :raiseable => 5, :env => 6
end
class MethodTable
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  def keys; Ruby.asm "push self\npush 1\nfetch_field"; end
  def values; Ruby.asm "push self\npush 2\nfetch_field"; end
  def bins; Ruby.asm "push self\npush 3\nfetch_field"; end
  def entries; Ruby.asm "push self\npush 4\nfetch_field"; end
  def default; Ruby.asm "push self\npush 5\nfetch_field"; end
  ivar_as_index :__ivars__ => 0, :keys => 1, :values => 2, :bins => 3, :entries => 4, :default => 5
end
class MatchData
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  def source; Ruby.asm "push self\npush 1\nfetch_field"; end
  def regexp; Ruby.asm "push self\npush 2\nfetch_field"; end
  def full; Ruby.asm "push self\npush 3\nfetch_field"; end
  def region; Ruby.asm "push self\npush 4\nfetch_field"; end
  ivar_as_index :__ivars__ => 0, :source => 1, :regexp => 2, :full => 3, :region => 4
end
class IO
  def descriptor; Ruby.asm "push self\npush 0\nfetch_field"; end
  ivar_as_index :descriptor => 0
end
class Object
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  ivar_as_index :__ivars__ => 0
end
class Regexp
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  def source; Ruby.asm "push self\npush 1\nfetch_field"; end
  def data; Ruby.asm "push self\npush 2\nfetch_field"; end
  def names; Ruby.asm "push self\npush 3\nfetch_field"; end
  ivar_as_index :__ivars__ => 0, :source => 1, :data => 2, :names => 3
end
