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
