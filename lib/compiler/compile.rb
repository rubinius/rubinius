$: << File.expand_path(File.dirname(__FILE__))

require 'compiler'
require 'generator'
require 'bytecode'
require 'text'

require 'options'


# "Interactive" mode
def interactive
  require 'readline'
  
  c = Compiler.new(Compiler::Generator)
  puts "Enter ? for help, ^D to exit."

  while code = Readline.readline("rbx:compile> ")
    if code == "?"
      puts "Enter any valid Ruby expression to have it compiled"
      next
    end

    cm = c.into_script(code.to_sexp).to_description.to_cmethod

    puts ""
    puts cm.decode
    puts ""

    cm.activate MAIN, Object, []
  end

  exit
end

# "Batch" mode
def batch(opts)
  c = Compiler.new(Compiler::Generator)
  verbose = opts['verbose']

  # Loopty-doop
  opts[:args].each do |file|
    next unless File.file? file

    puts "Compiling #{file}...\n"

    puts "  Parsing #{file}...\n" if verbose
    x = File.to_sexp(file)
    puts x.indented_inspect if verbose

    puts "  Generating AST...\n" if verbose
    n = c.into_script(x)

    puts "  Generating bytecode...\n" if verbose
=begin
#TODO: Figure out why text generation breaks subsequent compilation
    if verbose
      old_gen = c.generator_class
      c.generator_class = Compiler::TextGenerator
      txt = Compiler::TextGenerator.new
      n.bytecode(txt)
      txt.close
      puts txt.text
      c.generator_class = old_gen
    end
=end
    meth = n.to_description

    puts "  Encoding..." if verbose
    cm = meth.to_cmethod

    if verbose
      puts "\n  Decoded:\n" 
      puts "  ========\n"
      puts "  " << cm.decode.join("\n  ")
    end
    
    if opts['output']
      out = opts['output'].first
      puts "  Saving to '#{out}'...\n"
      Marshal.dump_to_file cm, out, Rubinius::CompiledMethodVersion
    end

    # This is pretty pointless until we actually start compiling to file
    if opts['execute']
      # cm.compile    # No such method..?
      puts "\n  EXECUTING:\n\n"
      cm.activate MAIN, Object, []
    end
  end
end


HEADER = <<-DATA
Usage:  shotgun/rubinius compiler2/compile.rb [OPTIONS] [filename.rb]

        Omitting the filename also gives the interactive prompt.
        The prompt will automatically execute the code.

DATA

o = Options.new do |o|
  o.header HEADER
  o.option '-x --execute      Execute code when done'
  o.option '-v --verbose      Print diagnostic info'
  o.option '-i --interactive  Present prompt'
  o.option '-h --help         Show this help message.'
  o.option '-o --output       Where to save the compiled file', :one

  o.on_error {|opt, ex| $stderr.puts opt.usage; exit 1 }
end

interactive if ARGV.empty?

opts = o.parse ARGV
(puts o.usage; exit) if opts['help']

interactive if opts['interactive'] or opts[:args].empty?
batch opts
