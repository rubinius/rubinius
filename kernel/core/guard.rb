# behaves like a channel read by a read-guarded replicating process
class Guard
  @@supervisor = Channel.new
  Thread.new do
    loop do
      begin
        value, action = @@supervisor.receive
        action.call value
      rescue Exception
      end
    end
  end

  alias :new! :new

  def new(&action)
    new! { |value| Thread.new { action.call value } }
  end

  def initialize(&action)
    @action = action
  end

  def send(value)
    @@supervisor.send [ value, @action ]
  end
  alias :<< :send
end
