##
# Stores global variables and global variable aliases.

module Rubinius
  class GlobalVariables
    def initialize
      load_path = %w[.]
      loaded_features = LoadedFeatures.new

      @internal = LookupTable.new
      @internal[:$;] = nil
      @internal[:$/] = "\n"             # Input record separator
      @internal[:$\] = nil              # Output record separator
      @internal[:$>] = STDOUT
      @internal[:$<] = ARGF
      @internal[:$:] = load_path
      @internal[:$-I] = load_path
      @internal[:$"] = loaded_features
      @internal[:$,] = ''               # Output field separator
      @internal[:$.] = 0                # TODO: Last line number of IO read.
      @internal[:$_] = nil              # HACK: bunk for now.
      @internal[:$<] = nil              # HACK: should be ARGF
      @internal[:$?] = nil              # Process status. nil until set
      @internal[:$=] = false            # ignore case, whatever that is
      @internal[:$CONSOLE]         = STDOUT
      @internal[:$DEBUG]           = false
      @internal[:$LOADED_FEATURES] = loaded_features
      @internal[:$LOAD_PATH]       = load_path
      @internal[:$SAFE]            = 0
      @internal[:$FILENAME]        = '-'
      @internal[:$stderr]          = STDERR
      @internal[:$stdin]           = STDIN
      @internal[:$stdout]          = STDOUT
      @internal[:$deferr]          = STDERR

      @alias = LookupTable.new
      @hooks = LookupTable.new
    end

    def key?(key)
      @internal.key?(key) || @alias.key?(key)
    end

    def variables
      @internal.keys + @alias.keys
    end

    def [](key)
      if @internal.key? key
        @internal[key]
      elsif @hooks.key? key
        @hooks[key][0].call(key)
      elsif @alias.key? key
        @internal[@alias[key]]
      end
    end

    def []=(key, data)
      if !@internal.key?(key) && alias_key = @alias[key]
        @internal[alias_key] = data
      elsif @hooks.key? key
        @internal[key] = @hooks[key][1].call(key, data)
      else
        @internal[key] = data
      end
    end

    def add_alias(from, to)
      if hook = @hooks[from]
        @hooks[to] = hook
      elsif alias_key = @alias[from]
        @alias[to] = alias_key
      else
        @alias[to] = from
      end
    end

    def illegal_set(*args)
      raise RuntimeError, "unable to set global"
    end

    def set_hook(var, getter=nil, setter=nil, &block)
      if block
        if getter or setter
          raise ArgumentError, "duplicate procs provided with block"
        end

        @hooks[var] = [block, method(:illegal_set)]
      else
        unless getter.respond_to?(:call)
          raise ArgumentError, "getter must respond to call"
        end

        if !setter
          setter = method(:illegal_set)
        elsif !setter.respond_to?(:call)
          raise ArgumentError, "setter must respond to call"
        end

        @hooks[var] = [getter, setter]
      end
    end
  end

  Globals = GlobalVariables.new
end
