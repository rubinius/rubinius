#
# = Timeout, timeout.rb
#
# Rubinius' implementation of the Ruby Stdlib Timeout API.
#



#
# Hard time limit for an arbitrary operation.
#
# The provided block of code is executed in a new Thread. If the
# block has not returned within the given number of seconds, the
# Thread it runs in is terminated by killing it.
#
# On success, true is returned. Otherwise, an error is raised in
# the Thread in which the call to Timeout was made. The error is
# either of the type supplied or, by default, Timeout::Error.
#
# The timeout may either be called directly through the module:
#
#
#   Timeout.timeout(60 * 30) {
#     Fun.start
#   }
#
#
# ...or through an includable instance method:
#
#
#   module Fun
#     include Timeout
#
#     def self.play_games(game)
#       timeout(60 * 30) { start }
#
#     rescue Timeout::Error
#       $stdout.puts i
#     end
#
#   end
#
module Timeout

  #
  # Simple error class with distinct type.
  #
  class Error < Interrupt; end


  #
  # Hard time limit for an arbitrary operation.
  #
  # Time in seconds after which the block is killed, and an
  # optional error class argument (defaults to Timeout::Error.)
  #
  # @todo   This is currently a blocking call in the calling
  #         thread. --rue
  #
  def timeout(in_this_many_seconds, error = Timeout::Error, &block)
    raise ThreadError, "Attempted during critical!" if Thread.critical

    t = Thread.new &block

    unless t.join in_this_many_seconds
      t.kill
      raise error, "Task #{t} not completed within #{in_this_many_seconds}s!"
    end

    t.value
  end

  module_function :timeout
end


#
# Legacy API
#


class TimeoutError < Timeout::Error; end

module Kernel
  def timeout(seconds, error = Timeout::Error, &block)
    Timeout.timeout seconds, error, &block
  end
end

