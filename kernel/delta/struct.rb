# -*- encoding: us-ascii -*-

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
    # will accidentally shadow they're #initialize. So for now, only run
    # the specialize if we're trying new Struct's directly from Struct itself,
    # not a craptastic Struct subclass.

    return unless superclass.equal? Struct

    args = []
    0.upto(attrs.size-1) do |i|
      args << "a#{i} = nil"
    end

    assigns = []
    0.upto(attrs.size-1) do |i|
      assigns << "@#{attrs[i]} = a#{i}"
    end

    hashes = []
    vars = []

    0.upto(attrs.size-1) do |i|
      hashes << "@#{attrs[i]}.hash"
      vars << "@#{attrs[i]}"
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
      module_eval code
    rescue SyntaxError
      # SyntaxError means that something is wrong with the
      # specialization code. Just eat the error and don't specialize.
    end
  end
end
