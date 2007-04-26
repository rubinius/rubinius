class Join
  class << self
    alias :private_new :new
    private :private_new
  end

  def self.new(&block)
    raise ArgumentError, "No block given" unless block
    c = Class.new self

    c.instance_eval do
      @max = -1
      @indices = {}
      @chords = []
    end

    def c.chord(*names, &action)
      raise ArgumentError, "No channel names specified" if names.empty?
      raise ArgumentError, "No block given" unless action
      names.map! { |n| n.to_sym }
      indices = names.map { |n| @indices[n] ||= ( @max += 1 ) }
      mask = indices.inject(0) { |acc, i| acc | ( 1 << i ) }
      @chords.push [ mask, indices, action ]
      names.each_with_index do |n, i|
        eval <<-EOS
          def #{ n }(value=nil)
            __channel_send__( #{ indices[i] }, value )
          end
        EOS
      end
      self
    end

    c.instance_eval &block
    c.instance_eval do
      @chords = @chords.sort_by { |mask, indices, action| -indices.size }
    end

    class << c
      undef :chord
      def new
        private_new(@chords, @max)
      end
    end

    c
  end

  def initialize(chords, max)
    @join_pending = (0..max).map { Array.new }
    @join_pending_mask = 0
    @join_chords = chords
    @join_lock = Channel.new
    @join_lock.send nil
  end

  def __channel_send__(index, value)
    @join_lock.receive
    begin
      @join_pending[index].push value
      @join_pending_mask |= 1 << index
      @join_chords.each do |mask, indices, action|
        if mask & @join_pending_mask == mask
          args = indices.map do |i|
            pending = @join_pending[i]
            value = pending.shift
            @join_pending_mask ^= 1 << i if pending.empty?
          end
          Thread.new { instance_exec(*args, &action) }
          break
        end
      end
    ensure
      @join_lock.send nil
    end
    self
  end
  private :__channel_send__
end
