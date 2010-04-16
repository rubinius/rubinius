module Rubinius

  # A simple command line option parser.
  class Options

    # A single command line option.
    class Option
      attr_reader :short, :long, :arg, :description, :block

      def initialize(short, long, arg, description, block)
        @short       = short
        @long        = long
        @arg         = arg
        @description = description
        @block       = block
      end

      def arg?
        @arg != nil
      end

      def optional?
        @arg[0] == ?[ and @arg[-1] == ?]
      end

      def match?(opt)
        opt == @short or opt == @long
      end
    end

    # Raised if incorrect or incomplete formats are passed to #on.
    class OptionError < Exception; end

    # Raised if an unrecognized option is encountered.
    class ParseError < Exception; end

    attr_accessor :config, :banner, :width, :options

    def initialize(banner="", width=30, config=nil)
      @parse    = true
      @banner   = banner
      @config   = config
      @width    = width
      @options  = []
      @doc      = []
      @extra    = []
      @align    = true
      @on_extra = lambda { |x|
        raise ParseError, "Unrecognized option: #{x}" if x[0] == ?-
        @extra << x
      }

      yield self if block_given?
    end

    # Documentation for options is left aligned. For example,
    #
    #  -a ARG       Some description
    #  --big        Another bit of info
    #  -c, --class  Yet more info
    #
    # See +option_align+.
    def left_align
      @align = nil
    end

    # Documentation for options is aligned by option type. For example,
    #
    #  -a ARG       Some description
    #      --big    Another bit of info
    #  -c, --class  Yet more info
    #
    # See +left_align+
    def option_align
      @align = true
    end

    # Registers an option. Acceptable formats for arguments are:
    #
    #  on "-a", "description"
    #  on "-a", "--abdc", "description"
    #  on "-a", "ARG", "description"
    #  on "--abdc", "ARG", "description"
    #  on "-a", "--abdc", "ARG", "description"
    #  on "-a", "[ARG]", "description"
    #
    # The [ARG] form specifies an optional argument. When parsing,
    # if an option having an optional argument is followed by a
    # registered option, +nil+ will be passed to the option having
    # an optional argument.
    #
    # If an block is passed, it will be invoked when the option is
    # matched. Not passing a block is permitted, but nonsensical.
    def on(*args, &block)
      raise OptionError, "option and description are required" if args.size < 2

      description = args.pop
      short, long, argument = nil
      args.each do |arg|
        if arg[0] == ?-
          if arg[1] == ?-
            long = arg
          else
            short = arg
          end
        else
          argument = arg
        end
      end

      add short, long, argument, description, block
    end

    # Adds documentation text for an option and adds an +Option+
    # instance to the list of registered options.
    def add(short, long, arg, description, block)
      pad = @align ? "  " : ""
      s = short ? short.dup : pad
      s << (short ? ", " : pad) if long
      doc "   #{s}#{long} #{arg}".ljust(@width-1) + " #{description}"
      @options << Option.new(short, long, arg, description, block)
    end

    # Searches all registered options to find a match for +opt+. Returns
    # +nil+ if no registered options match.
    def match?(opt)
      @options.find { |o| o.match? opt }
    end

    # Processes an option. Calles the #on_extra block (or default) for
    # unrecognized options. For registered options, possibly fetches an
    # argument and invokes the option's block if it is not nil.
    def process(argv, entry, opt, arg)
      unless option = match?(opt)
        @on_extra[entry]
      else
        if option.arg?
          if arg.nil?
            if option.optional?
              arg = argv.first

              if arg
                arg, equal_arg = arg.split("=",2)
              end

              if arg and arg[0] == ?-
                arg = match?(arg) ? nil : argv.shift
              else
                arg = argv.shift
              end
            else
              arg = argv.shift
            end
          end

          unless arg or option.optional?
            raise ParseError, "No argument provided for #{opt}"
          end

          option.block[arg] if option.block
        else
          option.block[] if option.block
        end
      end
      option
    end

    # Splits a string at +n+ characters into the +opt+ and the +rest+.
    # The +arg+ is set to +nil+ if +rest+ is an empty string.
    def split(str, n)
      opt  = str[0, n]
      rest = str[n, str.size] || ""
      arg  = rest == "" ? nil : rest
      return opt, arg, rest
    end

    # Parses an array of command line entries, calling blocks for
    # registered options.
    def parse(argv=ARGV)
      argv = Array(argv)

      while @parse and entry = argv.shift
        # collect everything that is not an option
        if entry[0] != ?-
          @on_extra[entry]
          next
        end

        # this is a long option
        if entry[1] == ?-
          opt, arg = entry.split "="
          process argv, entry, opt, arg
          next
        end

        # disambiguate short option group from short option with argument
        opt, arg, rest = split entry, 2

        # process first option
        option = process argv, entry, opt, arg
        next unless option and not option.arg?

        # process the rest of the options
        while rest.size > 0
          opt, arg, rest = split rest, 1
          opt = "-" + opt
          option = process argv, opt, opt, arg
          break if option.arg?
        end
      end

      @extra
    rescue ParseError => e
      puts self
      puts e
      exit 1
    end

    def start_parsing
      @parse = true
    end

    def stop_parsing
      @parse = false
    end

    # Adds a string of documentation text inline in the text generated
    # from the options. See #on and #add.
    def doc(str)
      @doc << str
    end

    # Convenience method for providing -v, --version options.
    def version(version, &block)
      show = block || lambda { puts "#{File.basename $0} #{version}"; exit }
      on "-v", "--version", "Show version", &show
    end

    # Convenience method for providing -h, --help options.
    def help(&block)
      help = block || lambda { puts self; exit 1 }
      on "-h", "--help", "Show this message", &help
    end

    # Stores a block that will be called with unrecognized options
    def on_extra(&block)
      @on_extra = block
    end

    # Returns a string representation of the options and doc strings.
    def to_s
      @banner + "\n\n" + @doc.join("\n") + "\n"
    end
  end
end
