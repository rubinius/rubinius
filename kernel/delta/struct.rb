class Struct
  Struct.new 'Tms', :utime, :stime, :cutime, :cstime, :tutime, :tstime

  class Tms
    def initialize(utime=nil, stime=nil, cutime=nil, cstime=nil,
                   tutime=nil, tstime=nil)
      @utime = utime
      @stime = stime
      @cutime = cutime
      @cstime = cstime
      @tutime = tutime
      @tstime = tstime
    end
  end

  def self._specialize(attrs)
    # Because people are crazy, they subclass Struct directly, ie.
    #  class Craptastic < Struct
    #
    # NOT
    #
    #  class Fine < Struct.new(:x, :y)
    #
    # When they do this craptastic act, they'll sometimes define their
    # own #initialize and super into Struct#initialize.
    #
    # When they do this and then do Craptastic.new(:x, :y), this code
    # will accidentally shadow their #initialize. So for now, only run
    # the specialize if we're trying new Struct's directly from Struct itself,
    # not a craptastic Struct subclass.

    return unless superclass.equal? Struct

    # To allow for optimization, we generate code with normal ivar
    # references for all attributes whose names can be written as
    # tIVAR tokens. For example, of the following struct attributes
    #
    #   Struct.new(:a, :@b, :c?, :'d-e')
    #
    # only the first, :a, can be written as a valid tIVAR token:
    #
    #   * :a can be written as @a
    #   * :@b becomes @@b and would be interpreted as a tCVAR
    #   * :c? becomes @c? and be interpreted as the beginning of
    #     a ternary expression
    #   * :'d-e' becomes @d-e and would be interpreted as a method
    #     invocation
    #
    # Attribute names that cannot be written as tIVAR tokens will
    # fall back to using #instance_variable_(get|set).

    args, assigns, hashes, vars = [], [], [], []

    attrs.each_with_index do |name, i|
      name = "@#{name}"

      if name =~ /^@[a-z_]\w*$/i
        assigns << "#{name} = a#{i}"
        vars    << name
      else
        assigns << "instance_variable_set(:#{name.inspect}, a#{i})"
        vars    << "instance_variable_get(:#{name.inspect})"
      end

      args   << "a#{i} = nil"
      hashes << "#{vars[-1]}.hash"
    end

    code = <<-CODE
      def initialize(#{args.join(", ")})
        #{assigns.join(';')}
        self
      end

      def hash
        hash = #{hashes.size}

        return hash if Thread.detect_outermost_recursion(self) do
          hash = hash ^ #{hashes.join(' ^ ')}
        end

        hash
      end

      def to_a
        [#{vars.join(', ')}]
      end

      def length
        #{vars.size}
      end
    CODE

    begin
      include Module.new do
        module_eval code
      end
    rescue SyntaxError
      # SyntaxError means that something is wrong with the
      # specialization code. Just eat the error and don't specialize.
    end
  end
end
