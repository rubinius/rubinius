class Struct
  Struct.new 'Tms', :utime, :stime, :cutime, :cstime

  class Tms
    def initialize(utime=nil, stime=nil, cutime=nil, cstime=nil)
      @utime = utime
      @stime = stime
      @cutime = cutime
      @cstime = cstime
    end
  end

  def self.specialize_initialize
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

    return unless self.equal? Struct

    attrs = self::STRUCT_ATTRS

    args = []
    0.upto(attrs.size-1) do |i|
      args << "a#{i} = nil"
    end

    assigns = []
    0.upto(attrs.size-1) do |i|
      assigns << "@#{attrs[i]} = a#{i}"
    end

    code = <<-CODE
      def initialize(#{args.join(", ")})
        #{assigns.join(';')}
        self
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
