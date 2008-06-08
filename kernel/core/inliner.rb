module Rubinius
module Inliner

## Really simple struct for our bytecode transformations below. Somewhere
## between the [[instr, args], ...] and a sequence of
## CompiledMethod::Instruction objects. In particular, can have multiple
## labels.
class Inst
  attr_accessor :op
  attr_accessor :args
  attr_accessor :labels

  def initialize op, args=[], labels=[]
    @op = op
    @args = [*args]
    @labels = labels
  end

  def size; args.size + 1 end

  ## sets of opcodes we perform transformations on
  OPCODES_WITH_LITERAL_0 = InstructionSet::OpCodes.select { |o| o.args[0] == :literal }.map { |o| o.opcode }
  OPCODES_WITH_LOCAL_0 = InstructionSet::OpCodes.select { |o| [:local, :block_local].include? o.args[0] }.map { |o| o.opcode }
  OPCODES_WITH_LOCAL_1 = InstructionSet::OpCodes.select { |o| [:local, :block_local].include? o.args[1] }.map { |o| o.opcode }
  OPCODES_WITH_IP_0 = InstructionSet::OpCodes.select { |o| o.args[0] == :ip }.map { |o| o.opcode }

  ## we have to subtrace this from OPCODES_WITH_LITERAL_0 when we're assuming
  ## sender == receipient, because we don't want to rewrite ivar access, but do
  ## want to rewrite other literals.
  OPCODES_WITH_IVAR_0 = [:push_ivar, :set_ivar]

  def literal0?; OPCODES_WITH_LITERAL_0.include? @op end
  def local0?; OPCODES_WITH_LOCAL_0.include? @op end
  def local1?; OPCODES_WITH_LOCAL_1.include? @op end
  def ip0?; OPCODES_WITH_IP_0.include? @op end
  def literal_not_ivar0?; (OPCODES_WITH_LITERAL_0 - OPCODES_WITH_IP_0).include? @op end

  def to_s
    (@labels.empty? ? "" :  "[#{@labels * ','}:] ") + "#@op(#{@args * ', '})"
  end
end

class Rejection < StandardError; end

class Base
  def reject s
    raise Rejection, s
  end

  def normalize_method m
    case m
    when DelegatedMethod
      m.receiver.method
    ## TODO: handle m being a DelegatedMethod
    when AccessVarMethod
      ## Right now this depends on AccessVarMethod#name, which seems a bit
      ## tenuous of a link. But we have to get the setter vs getter aspect of
      ## it somewhere, and that's the only place it's encoded right now.

      case m.name.to_s
      when /^(.+)=$/
        ivar = $1
        ops = [[:check_argcount, 1, 1],
               [:set_local_from_fp, 0, 0],
               [:push_local, 0],
               [:set_ivar, 0],
               [:sret]]
        CompiledMethod.from_bytecodes ops, 1, 1, Tuple["@#{ivar}".intern]
      else
        ops = [[:check_argcount, 0, 0],
               [:push_ivar, 0],
               [:sret]]
        CompiledMethod.from_bytecodes ops, 0, 0, Tuple["@#{m.name}".intern]
      end
    when DelegatedMethod
      reject "DelegatedMethods not yet supported"
    else
      m
    end
  end

  def debug s=nil
    return unless $DEBUG
    puts (s ? "## #{s}" : "")
  end

  ## simple debug dumper for Inst sequences
  def dump m, offset=0
    return unless $DEBUG
    m.inject(offset) do |ip, inst|
      printf "%6s %3d %s %s\n", (inst.labels.empty? ? "" : (inst.labels * ',') + ":"), ip, inst.op, (inst.args * ', ')
      ip + inst.size
    end
  end

  def dump2 m
    return unless $DEBUG
    m.inject(0) do |ip, (op, *args)|
      printf "%3d %s %s\n", ip, op, (args * ", ")
      ip + args.size + 1
    end
  end

  ## rewrite any srets as jumps to a target label
  def rewrite_srets_as_jumps_to m, target
    m.map do |inst|
      case inst.op
      when :sret
        debug "rewriting #{inst} to goto(#{target})"
        Inst.new :goto, target
      else
        inst
      end
    end
  end

  ## rewrite a bytecode sequence to update all literal and local references by
  ## adding the given offsets
  def rewrite_locs_and_lits m, local_offset, literal_offset
    m.each do |inst|
      if inst.local0?
        debug "rewriting local0 #{inst} +#{local_offset}"
        inst.args[0] += local_offset
      elsif inst.local1?
        debug "rewriting local1 #{inst} +#{local_offset}"
        inst.args[1] += local_offset
      elsif inst.literal_not_ivar0?
        debug "rewriting literal0 #{inst} +#{literal_offset}"
        inst.args[0] += literal_offset
      elsif inst.op == :set_local_from_fp
        reject "unexpeced set_local_from_fp: #{op.inspect}"
      elsif inst.op == :activate_method
        reject "no idea what to do with :activate_method"
      end
    end
  end

  ## Transform CompiledMethods into sequences of Insts. Also takes all IP
  ## references (jumps, exception-handling regions, line-to-ip regions) and
  ## adds them as pseudo-labels to the target Inst.
  def decompose method, start_label=0
    labels = {}
    next_label = start_label

    ## make insts objects, and record jump targets
    insts = method.bytecodes.decode.map do |op, *args|
      i = Inst.new op, args

      if i.ip0? # a jump of some sort
        dest = args.first
        l = (labels[dest] ||= (next_label += 1) - 1)
        i.args = [l]
      end
      i
    end

    ## store exception handler targets
    excs = method.exceptions.map do |tuple|
      lowip, highip, handleip = tuple.to_a

      lowl = (labels[lowip] ||= (next_label += 1) - 1)
      highl = (labels[highip] ||= (next_label += 1) - 1)
      handlel = (labels[handleip] ||= (next_label += 1) - 1)
      debug "mapped exception: #{[lowip, highip, handleip].inspect} => #{[lowl, highl, handlel].inspect}"
      [lowl, highl, handlel]
    end

    ## finally, store labels on the insts themselves
    ip = 0
    insts.each do |i|
      ## bah. apparently exception high-ip markers are not aligned at
      ## instruction boundaries. so we search for labels assigned to
      ## intra-intruction ips and add them to the lower instruction.
      ##
      ## we'll have to reverse this process at recomposition time, of course.
      (ip ... (ip + i.size)).each do |ipfreely|
        i.labels << labels[ipfreely] if labels[ipfreely]
      end
      ip += i.size
    end

    [insts, excs, next_label]
  end

  ## transform a sequence of Insts into bytecodes
  def recompose m, excs
    labels = {}

    ## first pass: get labels
    m.inject(0) do |ip, inst|
      inst.labels.each { |l| labels[l] = [ip, inst] }
      ip + inst.size
    end

    ## second pass: rewrite
    m = m.map do |inst|
      if inst.ip0? # a jump of some sort
        l = labels[inst.args.first] or reject "can't map label for #{inst} (inliner bug!)"
        [inst.op, l.first]
      else
        [inst.op, *inst.args]
      end
    end

    ## rewrite exceptions. for the high IP, shift to right before the next instruction
    ## boundary. not sure if that's actually important, but that's how they come in
    ## from the compiler...
    excs = excs.map do |low, high, handle|
      lowl, lowip = labels[low]
      reject "can't map low label #{low} for exception (inliner bug!)" unless lowl
      highl, highip = labels[high]
      reject "can't map high label #{high} for exception (inliner bug!)" unless highl
      handlel, handleip = labels[handle]
      reject "can't map handle label #{handle} for exception (inliner bug!)" unless handlel
      [lowl, highl + highip.size - 1, handlel]
    end.sort_by { |low, high, handle| high - low } # smallest first

    [m, excs]
  end
end

## Inliner for the simple case where you have the following calling
## instructions:
##   push_self
##   set_call_flags 1
##   send_method X OR send_stack X Y
##
## In other words, where the callee is guaranteed to be self, and the number of
## arguments is fixed (no default arguments and no splats).
class SimpleSelfCallInliner < Base
  def inline sender, receiver, sendsite_point
    sender = normalize_method sender
    receiver = normalize_method receiver
    sops, sexcs, num_labels = decompose sender
    rops, rexcs, num_labels = decompose receiver, num_labels

    debug "sender:"
    dump sops
    debug
    debug "receiver:"
    dump rops
    debug

    ## find & verify sendsite instructions based on sendsite_point
    injp, injp_size = verify_inject_point sops, sendsite_point
    debug "injection point is #{injp}...#{injp + injp_size}"

    ## determine number of arguments (currently, must be fixed)
    inst = sops[injp + injp_size - 1]
    num_args = case inst.op
    when :send_method; 0
    when :send_stack; inst.args[1]
    else
      reject "injection point #{p} instruction not send_stack or send_method: #{sops[p].inspect}"
    end
    debug "declared num args is #{num_args}"

    ## split sender around injection point
    sops_top = sops[0 ... injp]
    sops_bot = sops[(injp + injp_size) .. -1]

    ## rewrite receiver for argument passing. this elides the final
    ## instruction, so if it was a jump target, we need to attach that label
    ## below.
    rops, final_labels = rewrite_arguments rops, num_args

    ## make our own label for the final instruction, in case there wasn't one
    ## in the sender
    final_labels << num_labels
    debug "final labels are #{final_labels.inspect}"

    ## attach final labels to beginning of sender bottom
    sops_bot.first.labels += final_labels

    ## find any labels on the injection point and move them to the receiver's
    ## first instruction, since the injection point opcodes will be replaced by
    ## the receiver.
    inject_labels = sops[injp ... (injp + injp_size)].map { |i| i.labels }.flatten
    rops.first.labels += inject_labels
    debug "injection point labels are #{inject_labels.inspect}"

    ## rewrite receiver local and literal references. just shift them past the
    ## end of the sender counts.
    debug "sender has #{sender.local_count} locals and #{sender.literals.size} literals"
    rops = rewrite_locs_and_lits rops, sender.local_count, sender.literals.size

    ## rewrite receiver srets -> jumps to the receiver final label
    rops = rewrite_srets_as_jumps_to rops, final_labels.first

    ## dump
    offset = dump sops_top
    debug "<<< insplice >>>"
    offset = dump rops, offset
    debug "<<< outsplice >>>"
    offset = dump sops_bot, offset

    ## create bytecodes and exception tuple for the compiledmethod
    bytecodes, excs, = recompose((sops_top + rops + sops_bot), (sexcs + rexcs))

    dump2 bytecodes
    debug "exceptions: #{excs.inspect}"

    ## ennestedtupleize
    excs = Tuple[*excs.map { |e| Tuple[*e] }]

    c = CompiledMethod.from_bytecodes bytecodes, sender.required,
      (sender.local_count + receiver.local_count),
      (sender.literals + receiver.literals),
      excs
    c.file = sender.file # well, what am i supposed to do?
    c.name = sender.name
    c.path = sender.path
    c
  end

  def verify_inject_point sops, sendsite_point
    p = (0 ... sops.length).inject(0) do |ip, inum|
      break inum if ip == sendsite_point
      ip + sops[inum].size
    end

    if p.nil?
      reject "couldn't find injection point"
    end

    if p < 2 # must have at least two instructions beforehand
      reject "injection point #{p} too early"
    end

    if p >= sops.length
      reject "injection point #{p} too late for a #{sops.size}-instruction method"
    end

    ## here we determine the number of instructions (not the bytecode size!)
    ## that make up this call sequence we start with 1 for the send instruction
    ## itself
    size = 1

    if sops[p - size].op == :set_call_flags
      size += 1 # consume this op
    end

    unless sops[p - size].op == :push_self
      reject "injection point #{p}-#{size} is not [:push_self]: #{sops[p - size].inspect}"
      size += 1
    end

    [p - size, size + 1] # right bound is exclusive
  end

  def rewrite_arguments m, num_args
    ## drop argcount check
    unless m.first.op == :check_argcount && m.first.args == [num_args, num_args]
      reject "first opcode of m is not [:check_argcount, #{num_args}, #{num_args}]"
    end
    m.shift

    ## drop final return
    unless m.last.op == :sret
      reject "last opcode of method is not [:sret]"
    end
    ## we save the label and return it because we'll reattach it to the bottom
    ## half of the enclosing method
    final_labels = m.last.labels
    m.pop

    ## rewrite set_local_from_fp statements
    if m[num_args].op == :set_local_from_fp
      reject "unexpected :set_local_from_fp at m position #{num_args}: #{m[num_args.inspect]}"
    end

    next_fp = 0
    new_header = []

    m[0 ... num_args].each do |inst|
      unless inst.op == :set_local_from_fp && inst.args == [next_fp, next_fp]
        reject "expecting set_local_from_fp for #{next_fp}, found #{inst} instead"
      end
      new_header << Inst.new(:set_local, next_fp)
      new_header << Inst.new(:pop)
      next_fp += 1
    end

    [(new_header + m[num_args .. -1]), final_labels]
  end
end

end # module Inliner
end # module Rubinius
