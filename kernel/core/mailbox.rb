class Mailbox
  def initialize
    @channel = Channel.new
    @skipped = []
  end

  def send(value)
    @channel.send value
    self
  end
  alias :<< :send

  def receive
    if block_given?
      filter = Filter.new
      yield filter

      value = nil
      action = nil

      found_at = nil
      @skipped.each_with_index do |obj, index|
        action = filter.action_for obj
        if action
          value = obj
          found_at = index
          break
        end
      end
      @skipped.delete_at found_at if found_at

      until action
        value = @channel.receive
        action = filter.action_for value
        @skipped.push value unless action
      end

      action.call value
    else
      unless @skipped.empty?
        @skipped.shift
      else
        @channel.receive
      end
    end
  end

  class Filter
    def initialize
      @pairs = []
    end

    def on(pattern, &action)
      raise ArgumentError, "no block given" unless action
      @pairs.push [pattern, action]
    end

    def action_for(value)
      pair = @pairs.find { |pattern, action| pattern === value }
      pair ? pair[1] : nil
    end
  end
end
