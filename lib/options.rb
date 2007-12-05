# A simple, independent option parser originally from the rs project.
class Options

  # Create an instance. If a Hash of options is
  # given, it is passed to #configure.
  def initialize(message = '', &block)
    @allowed, @header, @optdesc = {}, message, ''

    block.call(self) if block
  end                                 # initialize

  # Adds an option to the parser. The option must be defined as 
  # a string of the format "-s --long Description". The first
  # element is a dash followed by a character (the "short option"),
  # the second is two dashes followed by two or more characters 
  # (the "long option"). The rest of the string becomes the third 
  # element which is used as the description of the option's 
  # purpose. The elements are separated by any number of whitespace 
  # characters.
  #
  # By default an option takes no arguments but a second argument
  # may be given to designate a different argument count. The
  # two last versions are greedy and will consume all arguments
  # until the next option switch or the end of input.
  #
  #   :none   no arguments
  #   :one    exactly one argument 
  #   :many   one or more
  #   :any    zero or more
  def option(definition, args = :none)
    result = definition.scan /-(\w)\s+--(\w\S+?)\s+?(\S.*)/
    raise ArgumentError, "Option format is '-s --long Description here'" if result.empty?

    short, long, desc = result.first

    # Store the data along with a cross-reference
    @allowed[short] = {:desc => desc, :args => args, :other => long}
    @allowed[long]  = {:desc => desc, :args => args, :other => short}

    # Add to usage now to maintain order
    argdesc   = case args
                  when :one then 'ARG'
                  when :any then '[ARG, ...]'
                  when :many then 'ARG1, ARG2[, ...]'
                  else         
                    ''
                end
                
    @optdesc << "        Options:\n\n" if @optdesc.empty?
    @optdesc << "        #{"-#{short} --#{long} #{argdesc}".ljust(30)} #{desc}\n" 
  end                                 # option

  # Optional error handling block
  def on_error(&block)
    @error_block = block
  end

  # Text to show above options in usage message
  def header(message)
    @header = message
  end                                 # header

  # Generate a usage message
  def usage()
    @header + @optdesc
  end                                 # usage

  alias :help :usage

  # The accepted forms for options are:
  #
  # Short opts: -h, -ht, -h ARG, -ht ARG (same as -h -t ARG).
  # Long opts:  --help, --help ARG, (No joining.)
  #
  # The returned Hash is indexed by the names of the found
  # options. These indices point to an Array of arguments
  # to that option if any, or just true if not. The Hash
  # also contains a special index, :args, containing all
  # of the non-option arguments.
  #
  # Upon encountering an error, the parser will raise an
  # ArgumentError with an explanation. This behaviour may
  # be overridden by supplying a block through #on_error.
  # The block will be given the Options instance and the
  # Exception object.
  def parse(arguments)
    expecting = nil     # An option may be expecting an argument
    @opts     = {}      # Options parsed
    @nonopts  = []      # Non-option arguments

    arguments = arguments.split unless arguments.kind_of?(Array)
    raise ArgumentError, "Empty input" if arguments.empty?

    arguments.each do |opt|
      # Option type
      if opt =~ /(-{1,2})(\S+)/
        # No more arguments for the previous option
        expecting = nil

        # Single args may need to be split --note: only the last one can take an arg!
        opts = if $1.length == 1
                $2.split(//)
               else
                [$2]
               end

        # Parse individual flags if any
        opts.each do |o|
          data = @allowed.fetch(o) {|x| raise ArgumentError.new("Invalid option #{x}")}

          # Option takes arguments?
          if [:one, :many].include? data[:args]
            # Prepare for incoming data
            @opts[o] ||= []; expecting = [o, data[:args]]
            
          elsif data[:args] == :any
            # Cheat
            @opts[o] = true; expecting = [o, :many] 

          else
            @opts[o] = true
          end                       # if [:yes, :many]
        end                         # opts.each

      # Nonoption arguments
      else
        # Previous option accepts arguments
        if expecting
          @opts[expecting.first] = [] if @opts[expecting.first] == true

          # Store the argument with the option
          @opts[expecting.first] << opt

          # No more arguments unless so specified
          expecting = nil unless expecting.last == :many

        # Freestanding argument
        else
          @nonopts << opt
        end                           # if expecting
      end                             # case opt
    end                               # arguments...each

    # Sanity checks and crossrefs
    @opts.keys.each do |key| 
      o = @opts[key]

      @opts[@allowed[key][:other]] = o

      case @allowed[key][:args]
        when :one
          if o.nil? or o == true or o.size != 1
            raise ArgumentError, "#{key} must have one argument" 
          end
        when :many
          if o.nil? or o == true or o.size < 2
            raise ArgumentError, "Too few arguments for #{key}" 
          end
      end
    end 

    @opts[:args] = @nonopts
    @opts

  rescue ArgumentError => ex
    raise ex unless @error_block
    @error_block.call self, ex
  end                                 # parse
end                                   # class Options
