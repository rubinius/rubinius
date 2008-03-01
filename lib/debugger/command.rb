class Debugger
  # Base class for debugger commands. Each subclass *must* implement the
  # following methods:
  # - command_regexp, which returns a Regexp instance used to determine when the
  # user is executing the corresponding command.
  # - execute, which takes two parameters: the instance of the +Debugger+, and a
  # +MatchData+ object containing the results of the user input matched against
  # the command_regexp.
  # Additionally, subclasses should also implement help, which should return two
  # strings: a specification of the command syntax, and a short bit of text
  # describing what the command does.
  #
  # Debugger::Command subclasses are registered with the Debugger::Command
  # superclass via an +inherited+ hook. Standard commands are defined in this
  # file, but additional debugger commands can be defined elsewhere and
  # required in.
  # However, this should be done *before* the Debugger is instantiated.
  #
  # TODO: Add a command to load extension commands after debugger is
  # instantiated
  class Command
    @commands = []

    def self.available_commands
      @commands
    end

    def self.inherited(klass)
      @commands << klass
    end

    # By default commands are processed in alphabetic order of the first item of
    # the help string. By overriding this method, commands can order themselves
    # in relation to other commands they need to precede or follow.
    def <=>(other)
      if other.kind_of? Command
        order = 0
        if other.public_methods(false).include? '<=>'
          order = (other <=> self) * -1
        end
        if order == 0
          cmd, = help
          oth_cmd, = other.help
          order = (cmd <=> oth_cmd)
        end
        return order
      else
        return nil
      end
    end
  end
end

# Include the standard debugger commands
require 'debugger/standard_commands'
require 'debugger/vm_commands'
