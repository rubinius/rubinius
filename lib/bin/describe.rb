require 'compiler/compiler'
require 'compiler/generator'
require 'compiler/bytecode'
require 'compiler/text'

require 'options'


# "Interactive" mode
def interactive()
  require 'readline'

  c = Compiler.new(Compiler::TextGenerator)
  puts "Enter ? for help, ^D to exit."

  while code = Readline.readline("rbx:describe> ")
    if code == "?"
      puts "Enter any valid Ruby expression to see its compilation process."
      next
    end

    code = code.to_sexp

    puts ""
    puts code.indented_inspect
    puts c.into_script(code).to_description.generator.text
    puts ""
  end

  exit
end

# "Batch" mode
def batch(opts)
  c = Compiler.new(Compiler::TextGenerator)

  # Loop through and parse everything
  opts[:args].each do |file|
    next unless File.file? file

    puts "\nParsing #{file}\n"
    puts "\n -- Sexp:\n"
    s = File.to_sexp file
    puts s.indented_inspect

    puts "\n -- Constructing AST:\n"
    n = c.into_script s
    puts n.describe_ast if opts['show-ast']

    puts "\n -- Generating bytecode:\n"
    meth = n.to_description
    puts ""
    puts meth.generator.text
  end
end



o = Options.new do |o|
      o.header "Usage:  shotgun/rubinius compiler2/describe.rb [FILE, ...]\n" <<
               "        Omitting the filename also gives the interactive prompt.\n" <<
               "\n"
      o.option '-a --show-ast     ASCII map of the AST'
      o.option '-i --interactive  Present prompt'
      o.option '-h --help         Show this help message.'

      o.on_error {|opt, ex| $stderr.puts opt.usage; exit 1 }
    end

interactive if ARGV.empty?

opts = o.parse ARGV
(puts o.usage; exit) if opts['help']

interactive if opts['interactive'] or opts[:args].empty?
batch opts

