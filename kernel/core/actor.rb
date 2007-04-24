class Actor
  class << self
    def spawn(&prc)
      channel = Channel.new
      Thread.new do
        channel << current
        prc.call
      end
      channel.receive
    end
    alias :new :spawn

    def current
      Thread.current[:__current_actor__] ||= Actor.new(current_mailbox)
    end

    def current_mailbox
      Thread.current[:__current_mailbox__] ||= Mailbox.new
    end
    private :current_mailbox

    def receive(&prc)
      current_mailbox.receive(&prc)
    end
  end

  def initialize(mailbox)
    @mailbox = mailbox
  end

  def send(value)
    @mailbox.send value
    self
  end
  alias :<< :send
end
