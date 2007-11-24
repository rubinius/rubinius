# A simple, independent option parser originally from the ruSH project.
class Options

  # Create an instance. If a Hash of options is
  # given, it is passed to #configure.
  def initialize(allowed_options = nil)
    @allowed, @usage = {}, ''
    configure allowed_options if allowed_options
  end                                 # initialize(allowed_options)

  # Add a single option to the parser given a short and long
  # name as well as a description. Arguments may be specified.
  #   :none (default)
  #   :one means exactly one argument 
  #   :many means more than one 
  #   :maybe means zero or more
  def option(short, long, desc, args = :none)
    raise ArgumentError.new('Option must have short, long and desc') unless short and long and desc

    short, long = short.delete('-'), long.delete('-')

    # Store the data along with a cross-reference
    @allowed[short] = {:desc => desc, :args => args, :other => long}
    @allowed[long]  = {:desc => desc, :args => args, :other => short}

    # Add to usage to maintain order
    argdesc   = case args
                  when :one; 'ARG'
                  when :many; 'ARG[, ...]'
                  when :maybe; '[ARG, ...]'
                  else;         ''
                end
                
    @usage << "     #{"-#{short} --#{long} #{argdesc}".ljust(30)} #{desc}\n" 
  end                                 # option

  # Generate a usage message
  def usage()
    @usage
  end                                 # usage

  # Parse arguments according to previously a given #configure.
  # The parser only accepts an Array, Strings have to be parsed
  # beforehand. The parser matches the given options to the ones
  # defined in #configure, leaves non-option arguments (such as
  # the path in 'ls -la /etc') alone. Any unknown arguments raise
  # an ArgumentError. The results of the parse are returned as a
  # Hash, see below. The accepted forms for options are:
  #
  # Short opts: -h, -ht, -h ARG, -ht ARG (same as -h -t ARG).
  # Long opts:  --help, --help ARG, (No joining.)
  #
  # Quoted strings are valid arguments but for Strings only.
  # Arrays should have parsed those at an earlier point.
  #
  # The returned Hash is indexed by the names of the found
  # options. These indices point to an Array of arguments
  # to that option if any, or just true if not. The Hash
  # also contains a special index, :args, containing all
  # of the non-option arguments.
  def parse(arguments)
    expecting = nil     # An option may be expecting an argument
    @opts     = {}      # Options parsed
    @nonopts  = []      # Non-option arguments

    arguments = arguments.split unless arguments.kind_of?(Array)

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
          # Valid options only
          data = @allowed.fetch(o) {|x| raise ArgumentError.new("Invalid option #{x}")}

          # Option takes arguments?
          if [:one, :many].include? data[:args]
            # Prepare for incoming data
            @opts[o] ||= []; expecting = [o, data[:args]]
            
          elsif data[:args] == :maybe
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
          raise ArgumentError, "#{key} must have one argument" if o.nil? or o == true or o.size != 1
        when :many
          raise ArgumentError, "Too few arguments for #{key}" if o.nil? or o == true or o.size < 2
      end
    end 

    @opts[:args] = @nonopts
    @opts
  end                                 # parse(arguments)
end                                   # class Options
