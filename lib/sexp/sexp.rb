##
# Sexps are the basic storage mechanism of SexpProcessor.  Sexps have
# a +type+ (to be renamed +node_type+) which is the first element of
# the Sexp. The type is used by SexpProcessor to determine whom to
# dispatch the Sexp to for processing.

$TESTING ||= false # unless defined $TESTING

class Sexp < Array # ZenTest FULL

  @@array_types = [ :array, :args, ]

  ##
  # Named positional parameters.
  # Use with +SexpProcessor.require_empty=false+.
  attr_accessor :accessors

  ##
  # Create a new Sexp containing +args+.

  def initialize(*args)
    @accessors = []
    super(args)
  end

  def self.from_array(a)
    ary = Array === a ? a : [a]

    result = self.new

    ary.each do |x|
      case x
      when Sexp
        result << x
      when Array
        result << self.from_array(x)
      else
        result << x
      end
    end

    result
  end

  def ==(obj) # :nodoc:
    if obj.class == self.class then
      super
    else
      false
    end
  end

  def ===(sexp)
    return nil unless Sexp === sexp
    pattern = self # this is just for my brain

    return true if pattern == sexp

    sexp.each do |subset|
      return true if pattern === subset
    end

    return nil
  end

  def =~(pattern)
    return pattern === self
  end

  ##
  # Returns true if the node_type is +array+ or +args+.
  #
  # REFACTOR: to TypedSexp - we only care when we have units.

  def array_type?
    type = self.first
    @@array_types.include? type
  end

  ##
  # Enumeratates the sexp yielding to +b+ when the node_type == +t+.

  def each_of_type(t, &b)
    each do | elem |
      if Sexp === elem then
        elem.each_of_type(t, &b)
        b.call(elem) if elem.first == t
      end
    end
  end

  ##
  # Replaces all elements whose node_type is +from+ with +to+. Used
  # only for the most trivial of rewrites.

  def find_and_replace_all(from, to)
    each_with_index do | elem, index |
      if Sexp === elem then
        elem.find_and_replace_all(from, to)
      else
        self[index] = to if elem == from
      end
    end
  end

  def gsub(pattern, repl)
    return repl if pattern == self

    new = self.map do |subset|
      case subset
      when Sexp then
        subset.gsub(pattern, repl)
      else
        subset
      end
    end

    return Sexp.from_array(new)
  end

  def inspect # :nodoc:
    sexp_str = self.map {|x|x.inspect}.join(', ')
    return "s(#{sexp_str})"
  end

  ##
  # Fancy-Schmancy method used to implement named positional accessors
  # via +accessors+.
  #
  # Example:
  #
  #   class MyProcessor < SexpProcessor
  #     def initialize
  #       super
  #       self.require_empty = false
  #       self.sexp_accessors = {
  #         :call => [:lhs, :name, :rhs]
  #       }
  #       ...
  #     end
  #   
  #     def process_call(exp)
  #       lhs = exp.lhs
  #       name = exp.name
  #       rhs = exp.rhs
  #       ...
  #     end
  #   end

  def method_missing(meth, *a, &b)
    super unless @accessors.include? meth

    index = @accessors.index(meth) + 1 # skip type
    return self.at(index)
  end

  def pretty_print(q) # :nodoc:
    q.group(1, 's(', ')') do
      q.seplist(self) {|v| q.pp v }
    end
  end

  ##
  # Returns the Sexp without the node_type.

  def sexp_body
    self[1..-1]
  end

  ##
  # If run with debug, Sexp will raise if you shift on an empty
  # Sexp. Helps with debugging.

  def shift
    raise "I'm empty" if self.empty?
    super
  end if $DEBUG or $TESTING

  ##
  # Returnes the bare bones structure of the sexp.
  # s(:a, :b, s(:c, :d), :e) => s(:a, s(:c))

  def structure
    result = self.class.new
    if Array === self.first then
      result = self.first.structure
    else
      result << self.shift
      self.grep(Array).each do |subexp|
        result << subexp.structure
      end
    end
    result
  end

  def sub(pattern, repl)
    return repl.dup if pattern == self

    done = false

    new = self.map do |subset|
      if done then
        subset
      else
        case subset
        when Sexp then
          if pattern == subset then
            done = true
            repl.dup
          elsif pattern === subset then
            done = true
            subset.sub pattern, repl
          else
            subset
          end
        else
          subset
        end
      end
    end

    return Sexp.from_array(new)
  end

  def to_a # :nodoc:
    self.map { |o| Sexp === o ? o.to_a : o }
  end

  def to_s # :nodoc:
    inspect
  end

end

class SexpMatchSpecial < Sexp; end

class SexpAny < SexpMatchSpecial
  def ===(o)
    return Sexp === o
  end

  def inspect
    "ANY"
  end
end

module SexpMatchSpecials
  def ANY(); return SexpAny.new; end
end

##
# This is just a stupid shortcut to make indentation much cleaner.

def s(*args)
  Sexp.new(*args)
end

