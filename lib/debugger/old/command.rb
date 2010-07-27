module Rubinius
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
      # Regular expression for matching a Ruby module or class name
      MODULE_RE = '((?:(?:[A-Z]\w*)(?:::)?)*(?:[A-Z]\w*))'

      # Regular expression for matching a Ruby method name
      METHOD_RE = '((?:[a-zA-Z0-9_]+[!?]?=?)|[+\-*\/%\^<>]|\*\*|\[\]=?|==|===|>=|<=|<=>|!=|<<|>>|&&|\|\|)'

      # Regular expression for matching a module and method name; defines three groups:
      # 1. The module/class (optional)
      # 2. The separator (a . or #)
      # 3. The method name
      MODULE_METHOD_RE = '(?:' + MODULE_RE + '([.#]))?' + METHOD_RE

      @@commands = []

      def self.available_commands
        @@commands
      end

      # Registers a command name
      def self.command(name, details={})
        @@commands << self

        define_method :name do
          name
        end

        unless re = details[:regexp] and re.kind_of? Regexp
          raise ArgumentError, "A regex representing the command syntax must be specified"
        end
        define_method :command_regexp do
          details[:regexp]
        end

        define_method :syntax do
          details[:syntax]
        end

        define_method :description do
          details[:description].gsub(/\s\s+/,' ').strip
        end
      end

      # Registers a command setting
      def self.setting(name, default, options={})
        @settings ||= {}
        @settings[name] = [default, options]
      end

      # Returns the settings that can be used with this command.
      def settings
        self.class.instance_variable_get("@settings") || {}
      end

      def initialize
        if settings = self.class.instance_variable_get("@settings")
          settings.each do |key, val|
            instance_variable_set "@#{key.to_s}", val.first
          end
        end
      end

      # Indicates whether command may take (require) multiple lines of input.
      # If this is true, a command must indicate to the debugger when additional
      # input is required, by calling the Debugger#more_input! method.
      def multiline?
        false
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
            order = (syntax <=> other.syntax)
          end
          return order
        else
          return nil
        end
      end

      # Returns a Method or UnboundMethod object, given strings that identify:
      # - the class/module (optional, defaults to MAIN if not specified)
      # - the method type (# for an instance method, . for a class method)
      #   (optional, default is to assume instance method)
      # - the method name
      # Note: The three strings can be obtained from user input via the use of the
      # MODULE_METHOD_RE constant defined on this class.
      def get_method(mod, mthd_type, mthd)
        clazz = MAIN.class
        unless mod.nil?
          clazz = Module.const_lookup(mod.to_sym)
        end
        if mthd_type.nil? || mthd_type == '#'
          cm = clazz.instance_method(mthd.to_sym)
        else
          cm = clazz.method(mthd.to_sym)
        end
      end
    end
  end
end

# Include the standard debugger commands
require 'debugger/standard_commands'
require 'debugger/vm_commands'
