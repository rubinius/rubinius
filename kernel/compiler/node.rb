#--
# See compiler.rb for more information about the Compiler mode of
# operation etc.
#++

class Compiler

  ##
  # Node is the representation of a node in the Abstract Syntax Tree
  # (AST) of the Ruby code. It is typically fairly close in structure
  # to the sexp produced by the parser but there are some transforms
  # that are done at this stage:
  #
  # 1. Compiler special forms, such as Rubinius.asm which allows
  #    inline Rubinius "assembly" code. See plugins.rb for more of
  #    these.
  # 2. Combining redundant and removing obsolete nodes from the tree.
  #    The current parser is still mostly MatzRuby's and therefore
  #    contains some artifacts that we have no need for.
  # 3. Optimizations. At this time, there are not that many nor will
  #    there ever be huge amounts.
  # 4. Sexp transformations, somewhat akin to Lisp macros. These
  #    allow modifying the sexp, and therefore the code produced,
  #    as it is being compiled. The mechanism is currently very raw
  #    and the only transform supported is conditional compilation
  #    (e.g. not including debug statements in the bytecode at all.)
  #    Look for Rubinius.compile_if.
  #
  # The compiler is based on the Visitor pattern, and this stage is no
  # different. First, for every type of sexp node possible in Ruby code,
  # one of these Node subclasses will provide a handler (even if to just
  # skip over the node.) The   #kind method provides this mapping; for
  # example, the Iter node is kind :iter. The nodes register the kinds
  # they provide for lookups.
  #
  # The general model is simple. Each node has a   #consume method, it
  # may either be the default one defined in Node or a custom one if one
  # is needed. When the sexp is fed here, the kind mapping is used to
  # determine the kind of Node to construct, and the rest of the sexp is
  # fed to it. Typically the Node then goes through what is next in the
  # sexp, asking each layer to construct itself and   #consume anything
  # even further down the chain all the way until nothing of the sexp
  # remains. At this point those recursive calls start rolling back up
  # higher and higher, and the Nodes at each layer compose themselves of
  # the constituent parts they get back from in exchange for the sexp
  # they provided. Verification happens here, to ensure the AST is sane.
  # Most optimization occurs at the point before the sub-sexp would be
  # processed; for example a normal Call node (:call is the normal
  # method call representation) would have its arguments and body sent
  # for processing, but if the Call detects a special form starting with
  # +Rubinius+, it produces the special code for that instead and may
  # even completely omit processing parts of the sexp that a normal
  # Call node would.
  #
  # Some Node classes are a bit fancier in their   #consume: ClosedScope
  # Node subclasses manage a scope for local variables and so on, for
  # example. The basic idea of having its children produce the subtree
  # underneath it and then organising those bits to send back to its
  # parent in turn is still the same, they just have additional things
  # they are responsible for.
  #
  # In the end, the caller will be left with a single top-level Node
  # object (usually a Script) which then hierarchically contains the
  # rest of the AST. This object graph can then be passed on to the
  # bytecode producer. See bytecode.rb for that.

  class Node
    Mapping = {}

    def self.kind(name=nil)
      return @kind unless name
      Mapping[name] = self
      @kind = name
    end

    def self.create(compiler, sexp)
      raise ArgumentError, "input must be a Sexp: #{sexp.inspect}" unless
        Sexp === sexp

      sexp.shift

      node = new(compiler)
      node.set_position sexp
      args = node.consume(sexp)
      msg = node.respond_to?(:normalize) ? :normalize : :args
      begin
        if node.respond_to? :normalize
          node = node.normalize(*args)
          node.set_position sexp
        else
          node.args(*args)
        end
      rescue ArgumentError => e
        e2 = ArgumentError.new e.message +
          ": #{node.class}##{msg} passed: #{args.inspect}"
        e2.set_backtrace e.backtrace
        raise e2
      end

      return node
    end

    def initialize(compiler)
      @compiler = compiler
      @body = nil
    end

    def set_position(sexp)
      @line = sexp.line
      @file = sexp.file
    end

    attr_reader :line, :file, :compiler

    def convert(x)
      @compiler.convert_sexp(x)
    end

    def consume(sexp)
      # This lets nil come back from convert_sexp which means
      # leave it out of the stream. This is primarily so that
      # expressions can be optimized away and wont be seen at
      # all in the output stream.
      out = []
      sexp.each do |x|
        if x.kind_of? Array
          v = @compiler.convert_sexp(x)
          out << v unless v.nil?
        else
          out << x
        end
      end

      return out
    end

    def args
    end

    def get(tag)
      @compiler.get(tag)
    end

    def set(tag, val=true, &b)
      @compiler.set(tag, val, &b)
    end

    def is?(clas)
      self.kind_of?(clas)
    end

    def expand(obj)
      obj.nil? ? Nil.new(@compiler) : obj
    end

    def use_plugin(g, kind, *args)
      @compiler.plugins[kind].find do |plug|
        plug.handle(g, self, *args)
      end
    end
  end
end
