unless defined? Channel
  require 'thread'
  class Channel < Queue
    alias receive shift
  end
end

module ThreadSpecs
  def self.sleeping_thread
    Thread.new do
      begin
        sleep
        ScratchPad.record :woken
      rescue Object => e
        ScratchPad.record e
      end
    end
  end

  def self.running_thread
    Thread.new do
      begin
        loop {}
        ScratchPad.record :woken
      rescue Object => e
        ScratchPad.record e
      end
    end
  end
end
