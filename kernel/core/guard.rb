# behaves like a channel read by a read-guarded replicating process
class Guard
  SUPERVISOR = Channel.new
  Thread.new do
    loop do
      begin
        value, action = SUPERVISOR.receive
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
    SUPERVISOR.send [ value, @action ]
  end
  alias :<< :send
end
