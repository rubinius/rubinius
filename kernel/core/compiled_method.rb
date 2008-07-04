# depends on: class.rb array.rb

##
# A wrapper for a calling a function in a shared library that has been
# attached via rb_define_method().
#
# The primitive slot for a NativeMethod points to the nmethod_call primitive
# which dispatches to the underlying C function.

class NativeMethod
  def lines
    nil
  end

  def exceptions
    nil
  end

  def literals
    nil
  end

  def line_from_ip(i)
    0
  end
end

##
# A linked list that details the static, lexical scope the method was created
# in.
#
# You can access it this way:
#
#   MethodContext.current.method.staticscope
#
# Here is a simple example:
#
#   module Fruits
#     class Pineapple
#       attr_reader :initialize_scope
#   
#       def initialize(weight)
#         @initialize_scope = MethodContext.current.method.staticscope
#         @weight = weight
#       end
#     end
#   end
#
# Static scope members are shown below:
#
#   irb(main):> pineapple.initialize_scope.script
#   => nil
#   irb(main):> pineapple.initialize_scope.parent
#   => #<StaticScope:0x1c9>
#   irb(main):> pineapple.initialize_scope.module
#   => Fruits::Pineapple
#   irb(main):> pineapple.initialize_scope.parent.module
#   => Fruits
#   irb(main):> pineapple.initialize_scope.parent.parent.module
#   => Object
#   irb(main):> pineapple.initialize_scope.parent.parent.parent.module
#   => Object

class StaticScope
  ivar_as_index :__ivars__ => 0, :module => 1, :parent => 2

  def initialize(mod, par=nil)
    @module = mod
    @parent = par
  end

  attr_accessor :script

  # Source code of this scope.
  def script
    @script
  end

  # Module or class this lexical scope enclosed into.
  def module
    @module
  end

  # Static scope object this scope enclosed into.
  def parent
    @parent
  end

  def inspect
    "#<#{self.class.name}:0x#{self.object_id.to_s(16)} parent=#{@parent} module=#{@module}>"
  end

  def to_s
    self.inspect
  end
end

##
# CompiledMethod represents source code method compiled into VM bytecodes.
# Its instruction set is then executed by Shotgun's abstraction of CPU.
# CompiledMethods are not just sets of instructions though. They carry a lot
# of information about method: its lexical scope (static scope), name, file
# it has been defined in and so forth.

class CompiledMethod
  # TODO: Delete/reuse cache (field 14) field from C structure
  ivar_as_index :__ivars__ => 0,
                :primitive => 1,
                :required => 2,
                :serial => 3, 
                :bytecodes => 4, 
                :name => 5, 
                :file => 6, 
                :local_count => 7, 
                :literals => 8, 
                :args => 9, 
                :local_names => 10, 
                :exceptions => 11, 
                :lines => 12, 
                :path => 13, 
                :metadata_container => 15, 
                :compiled => 16, 
                :staticscope => 17

  def __ivars__  ; @__ivars__  ; end

  ##
  # nil if the method does not have a primitive, otherwise the name of the
  # primitive to run.

  def primitive  ; @primitive  ; end

  # number of arguments required by method
  def required   ; @required   ; end

  # Version of method: an incrementing integer.
  # When you redefine method via re-opening
  # a class this number is increased.
  #
  # Kernel methods have serial of 0
  # %99.9 of the time.
  def serial     ; @serial     ; end

  # instructions set that VM executes
  # instance of InstructionSequence
  def bytecodes  ; @bytecodes  ; end

  # method name as Symbol
  def name       ; @name       ; end

  # file in which this method has been defined
  def file       ; @file       ; end

  # number of local variables method uses
  # note that locals are stored in slots
  # in the context this CompiledMethod
  # is executed in.
  def local_count; @local_count; end

  # literals tuple stores literals from
  # source code like string literals and
  # some extra stuff like SendSites,
  # RegExp objects created from
  # regexp literals and CompiledMethods
  # of inner methods.
  def literals   ; @literals   ; end

  # Tuple holding the arguments defined on a method.
  # Consists of 3 values:
  # - a tuple of symbols naming required args (or nil if none)
  # - a tuple of symbols naming optional args (or nil if none)
  # - the symbol for any splat arg (or nil if none)
  def args       ; @args       ; end

  # Tuple holding the symbols for all local variable names used in the method.
  def local_names; @local_names; end

  # Tuple of tuples. Inner tuples contain
  # low IP, high IP and IP of exception
  # handler.
  #
  # When exception is raised this tuple is
  # looked up by VM using context IP:
  #
  # Tuple which low/high IP fit in context
  # IP is picked up and handling continues.
  #
  # TODO: double check this statement.
  def exceptions ; @exceptions ; end

  # Tuple of Tuples. Each inner Tuple
  # stores the following information:
  #
  # low IP, high IP and line number as integer.
  def lines      ; @lines      ; end

  # Holds the path of a script CompiledMethod created using eval.
  # Required for the proper functioning of __FILE__ under eval.
  def path       ; @path       ; end

  # Separate object for storing metadata; this way
  # the metadata can change without changes to the
  # CM itself.
  def metadata_container      ; @metadata_container      ; end

  # ByteArray of pointers to optcodes.
  # This is only populated when CompiledMethod
  # is loaded into VM and platform specific.
  #
  # You can think of it as of internal
  # bytecode representation optimized
  # for platform Rubinius runs on.
  def compiled   ; @compiled   ; end

  # lexical scope of method in source
  # instance of StaticScope
  def staticscope; @staticscope; end

  ##
  # This is runtime hints, added to the method by the VM to indicate how it's
  # being used.

  attr_accessor :hints

  def inspect
    "#<#{self.class.name}:0x#{self.object_id.to_s(16)} name=#{@name} file=#{@file}>"
  end

  def from_string(bc, lcls, req)
    @bytecodes = bc
    @primitive = -1
    @local_count = lcls
    @literals = Tuple.new(0)
    @exceptions = nil
    @lines = nil
    @file = nil
    @name = nil
    @path = nil
    @required = req
    return self
  end

  def self.from_bytecodes bytecodes, arg_count, local_count, literals, exceptions=nil, lines=nil
    c = CompiledMethod.new
    c.bytecodes = InstructionSequence::Encoder.new.encode_stream bytecodes
    c.primitive = false
    c.local_count = local_count
    c.required = arg_count
    c.literals = literals
    c.lines = lines || Tuple[Tuple[0, bytecodes.size, 0]]
    c.exceptions = exceptions || []
    c
  end

  def inherit_scope(other)
    if ss = other.staticscope
      @staticscope = ss
    else
      @staticscope = StaticScope.new(Object)
    end
  end

  def staticscope=(val)
    raise TypeError, "not a static scope: #{val.inspect}" unless val.kind_of? StaticScope
    @staticscope = val
  end

  def exceptions=(tup)
    @exceptions = tup
  end

  def local_count=(val)
    @local_count = val
  end

  def required=(val)
    @required = val
  end

  def literals=(tup)
    @literals = tup
  end

  def args=(tup)
    @args = tup
  end

  def file=(val)
    @file = val
  end

  def name=(val)
    @name = val
  end

  def lines=(val)
    @lines = val
  end

  def path=(val)
    @path = val
  end

  def primitive=(idx)
    @primitive = idx
  end

  def serial=(ser)
    @serial = ser
  end

  def metadata_container=(tup)
    @metadata_container = tup
  end

  def args=(ary)
    @args = ary
  end

  def local_names=(names)
    return if names.nil?

    unless names.kind_of? Tuple
      raise ArgumentError, "only accepts a Tuple"
    end

    names.each do |n|
      unless n.kind_of? Symbol
        raise ArgumentError, "must be a tuple of symbols: #{n.inspect}"
      end
    end

    @local_names = names
  end

  def activate(recv, mod, args, locals=nil, &prc)
    sz = args.total
    if prc
      block = prc.block
    else
      block = nil
    end

    out = Rubinius.asm(args, block, locals, sz, mod, recv) do |a,b,l,s,m,r|
      run a
      push_array
      run b
      run l
      run s
      run m
      push :self
      run r
      activate_method 0
    end

    return out
  end

  # Accessor for a hash of filenames (as per $" / $LOADED_FEATURES) to the
  # script CompiledMethod.
  def self.scripts
    @scripts ||= {}
  end
  
  # Helper function for searching for a CM given a file name; applies similar
  # search and path expansion rules as load/require, so that the full path to
  # the file need not be specified.
  def self.script_for_file(filename)
    if cm = self.scripts[filename]
      return cm
    end
    # ./ ../ ~/ /
    if filename =~ %r{\A(?:(\.\.?)|(~))?/}
      if $2    # ~ 
        filename.slice! '~/'
        return scripts["#{ENV['HOME']}/#{filename}"]
      else    # . or ..
        return scripts["#{File.expand_path filename}"]
      end
    # Unqualified
    else
      scripts = self.scripts
      $LOAD_PATH.each do |dir|
        if cm = scripts["#{dir}/#{filename}"]
          return cm
        end
      end
    end
    nil
  end

  class Script
    attr_accessor :path
  end

  def as_script(script=nil)
    script ||= CompiledMethod::Script.new
    yield script if block_given?

    Rubinius::VM.save_encloser_path

    # Setup the scoping.
    ss = StaticScope.new(Object)
    ss.script = script
    @staticscope = ss

    activate_as_script
    Rubinius::VM.restore_encloser_path
  end

  def line_from_ip(i)
    @lines.each do |t|
      start = t.at(0)
      nd = t.at(1)
      op = t.at(2)
      if i >= start and i <= nd
        return op
      end
    end
    return 0
  end

  # Returns the address (IP) of the first instruction in this CompiledMethod
  # that is on the specified line, or the address of the first instruction on
  # the next code line after the specified line if there are no instructions
  # on the requested line.
  # This method only looks at instructions within the current CompiledMethod;
  # see #locate_line for an alternate method that also searches inside the child
  # CompiledMethods.

  def first_ip_on_line(line)
    @lines.each do |t|
      if t.at(2) >= line
        return t.at(0)
      end
    end

    return -1
  end

  def bytecodes=(other)
    @bytecodes = other
  end

  def first_line
    @lines.each do |ent|
      return ent[2] if ent[2] > 0
    end

    return -1
  end

  def is_block?
    @name =~ /__(?:(?:\w|_)+)?block__/
  end

  # Convenience method to return an array of the child CompiledMethods from
  # this CompiledMethod's literals.

  def child_methods
    literals.select {|lit| lit.kind_of? CompiledMethod}
  end

  # Convenience method to return an array of the SendSites from
  # this CompiledMethod's literals.

  def send_sites
    literals.select {|lit| lit.kind_of? SendSite}
  end

  # Locates the CompiledMethod and instruction address (IP) of the first
  # instruction on the specified line. This method recursively examines child
  # compiled methods until an exact match for the searched line is found.
  # It returns both the matching CompiledMethod and the IP of the first 
  # instruction on the requested line, or nil if no match for the specified line
  # is found.

  def locate_line(line, cm=self)
    cm.lines.each do |t|
      if (l = t.at(2)) == line
        # Found target line - return first IP
        return cm, t.at(0)
      elsif l > line
        break
      end
    end
    # Didn't find line in this CM, so check if a contained
    # CM encompasses the line searched for
    cm.child_methods.each do |child|
      if res = locate_line(line, child)
        return res
      end
    end

    # No child method is a match - fail
    return nil
  end

  ##
  # Decodes the instruction sequence that is represented by this compileed
  # method. Delegates to InstructionSequence to do the instruction decoding,
  # but then converts opcode literal arguments to their actual values by looking
  # them up in the literals tuple.
  # Takes an optional bytecodes argument representing the bytecode that is to
  # be decoded using this CompiledMethod's locals and literals. This is provided
  # for use by the debugger, where the bytecode sequence to be decoded may not
  # exactly match the bytecode currently held by the CompiledMethod, typically
  # as a result of substituting yield_debugger instructions into the bytecode.
  def decode(bytecodes = @bytecodes)
    stream = bytecodes.decode(false)
    ip = 0
    args_reg = 0
    stream.map! do |inst|
      instruct = Instruction.new(inst, self, ip, args_reg)
      ip += instruct.size
      if instruct.opcode == :set_args
        args_reg = 0
      elsif instruct.opcode == :cast_array_for_args
        args_reg = instruct.args.first
      end
      instruct
    end

    # Add a convenience method to the array containing the decoded instructions
    # to convert an IP address to the index of the corresponding instruction
    def stream.ip_to_index(ip)
      if ip < 0 or ip > last.ip
        raise ArgumentError, "IP address is outside valid range of 0 to #{last.ip} (got #{ip})"
      end
      each_with_index do |inst, i|
        return i if ip <= inst.ip
      end
    end
    stream
  end

  ##
  # Calculates the minimum stack size required for this method.
  #
  # Returns two values:
  # * The minimum size stack required
  # * A flag indicating whether this is an exact size, or a minimum
  def min_stack_size
    dc = decode
    high_mark = 0
    exact = true
    dc.inject(0) do |sz,op|
      i,flg = op.stack_produced
      sz += i
      exact &&= flg
      i,flg = op.stack_consumed
      sz -= i
      exact &&= flg
      high_mark = sz if sz > high_mark
      sz
    end
    return high_mark, exact
  end

  # Represents virtual machine's CPU instruction.
  # Instructions are organized into instruction
  # sequences known as iSeq, forming body
  # of CompiledMethods.
  #
  # To generate VM optcodes documentation
  # use rake doc:vm task.
  class Instruction
    def initialize(inst, cm, ip, args_reg)
      @op = inst[0]
      @args = inst[1..-1]
      @args.each_index do |i|
        case @op.args[i]
        when :literal
          @args[i] = cm.literals[@args[i]]
        when :local
          # TODO: Blocks should be able to retrieve local names as well,
          # but need access to method corresponding to home context
          @args[i] = cm.local_names[args[i]] if cm.local_names and cm.name != :__block__
        when :block_local
          # TODO: Blocks should be able to retrieve enclosing block local names as well,
          # but need access to static scope
          @args[i] = cm.local_names[args[i]] if cm.local_names and args[0] == 0
        end
      end
      @ip = ip
      @line = cm.line_from_ip(ip)

      @stack_consumed = calculate_stack_usage(@op.stack_consumed, args_reg)
      @stack_produced = calculate_stack_usage(@op.stack_produced)
    end

    # Instruction pointer
    attr_reader :ip
    attr_reader :line

    ##
    # Returns the OpCode object

    # Associated OptCode instance.
    def instruction
      @op
    end

    ##
    # Returns the symbol representing the opcode for this instruction.

    def opcode
      @op.opcode
    end

    ##
    # Returns an array of 0 to 2 arguments, depending on the opcode.

    def args
      @args
    end

    def size
      @args.size + 1
    end

    ##
    # Returns the stack operands consumed by this instruction, as well as a flag
    # indicating whether this is an exact value (true) or a minimum (false).

    def stack_consumed
      @stack_consumed
    end

    ##
    # Returns the stack operands produced by this instruction, as well as a flag
    # indicating whether this is an exact value (true) or a minimum (false).

    def stack_produced
      @stack_produced
    end

    ##
    # Calculate the stack usage (pushes or pops) of this instruction.

    def calculate_stack_usage(code, args_reg=0)
      usage = code
      exact = true
      if code < 0
        usage = 0
        if code == -999
          exact = false
        else
          # Stack usage depends on opcode args
          code *= -1
          mult, code = code.divmod(100)
          arg, code = code.divmod(10)
          if arg >= 1 and arg <= 2
            # Opcode consumes/produces a multiple of the value in the specified
            # opcode arg
            usage += mult * args[arg-1]
          elsif arg == 3
            # Opcode consumes number of args specified in args register
            usage += mult * args_reg
            exact = false
          end
          usage += code
        end
      end
      return usage, exact
    end

    def to_s
      str = "%04d:  %-27s" % [@ip, opcode]
      str << @args.map{|a| a.inspect}.join(', ')
    end
  end
end

