require 'actor'

module Pingpong
  def self.run    
    loop do
      Actor.receive do |filter|
        filter.when(Rubinius::Tuple[:ping, Object]) do |message|
          _, actor = message
          actor << :pong
        end
      end
    end
  end
end

