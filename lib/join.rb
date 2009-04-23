class Join
  def self.new(&block)
    raise ArgumentError, "No block given" unless block
    c = Class.new self

    c.instance_eval do
      @max = -1
      @indices = {}
      @chords = []
      @sync = {}
    end

    def c.chord(*names, &action)
      raise ArgumentError, "No channel names specified" if names.empty?
      raise ArgumentError, "No block given" unless action
      names.map! { |n| n.to_sym }
      indices = __defchannels__(*names)
      mask = indices.inject(0) { |acc, i| acc | ( 1 << i ) }
      @chords.push [ mask, indices, action ].freeze
      names.each_with_index do |n, i|
      end
      self
    end

    def c.async(*names)
      names.map! { |n| n.to_sym }
      __defchannels__(*names).each { |index| @sync[index] = false }
      self
    end

    def c.sync(*names)
      names.map! { |n| n.to_sym }
      __defchannels__(*names).each { |index| @sync[index] = true }
      self
    end

    def c.__defchannels__(*names)
      indices = names.map { |name| ( @indices[name] ||= ( @max += 1 ) ) }
      names.each_with_index do |name, i|
        eval <<-EOS
          def #{ name }(value=nil)
            __channel_send__( #{ indices[i] }, value )
          end
        EOS
      end
      indices
    end

    class << c
      private :chord, :async, :sync, :__defchannels__
    end 

    c.class_eval(&block)
    c.instance_eval { @chords.reverse! }

    @chords.freeze
    @sync.freeze

    class << c
      remove_method :chord, :async, :sync, :__defchannels__
      def new(*args, &block)
        obj = allocate
        chords = @chords
        max = @max
        sync = @sync
        obj.instance_eval do
          @join_pending = (0..max).map { Array.new }
          @join_pending_mask = 0
          @join_chords = chords
          @join_sync = sync
          @join_lock = Rubinius::Channel.new
          @join_lock.send nil
          initialize(*args, &block)
        end
        obj
      end
    end

    c
  end

  def __channel_send__(index, value)
    if @join_sync[index]
      reply = Rubinius::Channel.new
      value = [value, reply]
    end
    @join_lock.receive
    begin
      @join_pending[index].push value
      @join_pending_mask |= 1 << index
      @join_chords.each do |mask, indices, action|
        if mask & @join_pending_mask == mask
          args = []
          waiting = []
          indices.each do |i|
            pending = @join_pending[i]
            value = pending.shift
            if @join_sync[i]
              args.push value.first
              waiting.push value.last
            else
              args.push value
            end
            @join_pending_mask ^= 1 << i if pending.empty?
          end
          thread = Thread.new { instance_exec(*args, &action) }
          waiting.each { |waiter| waiter.send thread }
          break
        end
      end
    ensure
      @join_lock.send nil
    end
    if @join_sync[index]
      reply.receive.value
    else
      self
    end
  end
  private :__channel_send__
end
