class Compiler
  class Marshal

    def unmarshal(str)
      if str.kind_of? String
        return unmarshal_io(StringIO.new(str))
      else
        return unmarshal_io(str)
      end
    end

    def unmarshal_io(io)
      kind = io.gets[0]
      case kind
      when ?t
        return true
      when ?f
        return false
      when ?n
        return nil
      when ?I
        return io.gets.to_i
      when ?d
        return io.gets.to_f
      when ?s
        count = io.gets.to_i
        str = io.read(count)
        io.read(1) # remove the \n
        return str
      when ?x
        count = io.gets.to_i
        str = io.read(count)
        io.read(1) # remove the \n
        return str.to_sym
      when ?S
        count = io.gets.to_i
        str = io.read(count)
        io.read(1) # remove the \n
        return SendSite.new(str.to_sym)
      when ?A
        count = io.gets.to_i
        obj = Array.new(count)
        0.upto(count - 1) { |i| obj[i] = unmarshal_io(io) }
        return obj
      when ?p
        count = io.gets.to_i
        obj = Tuple.new(count)
        0.upto(count - 1) { |i| obj[i] = unmarshal_io(io) }
        return obj
      when ?i
        count = io.gets.to_i
        seq = InstructionSequence.new(count)
        0.upto(count - 1) do |i|
          seq[i] = io.gets.to_i
        end
        return seq
      when ?l
        count = io.gets.to_i
        lt = LookupTable.new
        count.times do
          size = io.gets.to_i
          
          key = io.read(size)
          io.read(1) # remove the \n
          
          val = unmarshal_io(io)
          lt[key.to_sym] = val
        end

        return lt
      when ?M
        version = io.gets.to_i
        if version != 1
          raise "Unknown CompiledMethod version #{version}"
        end
        cm = CompiledMethod.new
        cm.__ivars__ = unmarshal_io(io)
        cm.primitive = unmarshal_io(io)
        cm.name = unmarshal_io(io)
        cm.iseq = unmarshal_io(io)
        cm.stack_size = unmarshal_io(io)
        cm.local_count = unmarshal_io(io)
        cm.required_args = unmarshal_io(io)
        cm.total_args = unmarshal_io(io)
        cm.splat = unmarshal_io(io)
        cm.literals = unmarshal_io(io)
        cm.exceptions = unmarshal_io(io)
        cm.lines = unmarshal_io(io)
        cm.file = unmarshal_io(io)
        cm.local_names = unmarshal_io(io)
        return cm
      else
        raise "Unknown type '#{kind.chr}'"
      end
        
    end

    def marshal(val)
      str = ""

      case val
      when TrueClass
        str << "t\n"
      when FalseClass
        str << "f\n"
      when NilClass
        str << "n\n"
      when Fixnum, Bignum
        str << "I\n#{val}\n"
      when String
        str << "s\n#{val.size}\n#{val}\n"
      when Symbol
        s = val.to_s
        str << "x\n#{s.size}\n#{s}\n"
      when SendSite
        s = val.name.to_s
        str << "S\n#{s.size}\n#{s}\n"
      when Tuple
        str << "p\n#{val.size}\n"
        val.each do |ele|
          str << marshal(ele)
        end
      when Array
        str << "A\n#{val.size}\n"
        val.each do |ele|
          str << marshal(ele)
        end
      when Float
        str << "d\n#{val}\n"
      when InstructionSequence
        str << "i\n#{val.size}\n"
        val.opcodes.each do |op|
          str << op.to_s << "\n"
        end
      when LookupTable
        str << "l\n#{val.size}\n"
        val.each do |k,v|
          str << "#{k.to_s.size}\n#{k}\n"
          str << marshal(v)
        end
      when CompiledMethod
        str << "M\n1\n"
        str << marshal(val.__ivars__)
        str << marshal(val.primitive)
        str << marshal(val.name)
        str << marshal(val.iseq)
        str << marshal(val.stack_size)
        str << marshal(val.local_count)
        str << marshal(val.required_args)
        str << marshal(val.total_args)
        str << marshal(val.splat)
        str << marshal(val.literals)
        str << marshal(val.exceptions)
        str << marshal(val.lines)
        str << marshal(val.file)
        str << marshal(val.local_names)
      else
        raise ArgumentError, "Unknown type: #{val.class}"
      end

      return str
    end
  end
end
