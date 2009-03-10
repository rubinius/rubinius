##
# Namespace for Rubinius internals.

module Rubinius
  AtExit = []

  def warn(warning)
    unless $VERBOSE.nil? then
      $stderr.write "warning: #{warning}\n"
    end
    nil
  end
  module_function :warn


  def self.received_signal(sig)
    Signal.run_handler(sig)
  end
end
