##
# Namespace for Rubinius internals.

module Rubinius
  AtExit = []

  def warn(warning)
    unless $VERBOSE.nil? then
      $stderr.write "#{MethodContext.current.sender.location}: warning: #{warning}\n"
    end
    nil
  end
  module_function :warn
end
