#!/usr/bin/env rbx

require 'bytecode/compiler'
require 'bytecode/rubinius'

# recognize a few simple options
def usage
  puts <<-USAGE
  Usage: sirb [options]
    sirb is a RCEPL (read, compile, execute, print, loop) program for rubinius

    Options: 
            -p   Print the parsed s-expression
            -x   Print the normalized s-expression.
            -s   Print assembly instructions.
            -b   Print bytecode encoding.
            -t   Print stats.
  USAGE
end

env = ENV['SIRB_ARGS']
if env
  env.split(" ").each do |part|
    ARGV << part
  end
end

simple_prompt = false

begin
  until ARGV.empty?
    arg = ARGV.shift
    case arg
    when '-h'
      usage
    when '-p'
      $show_parse = true
    when '-x'
      $show_sexp = true
    when '-s'
      $show_asm = true
    when '-b'
      $show_bytes = true
    when '-t'
      $show_stats = true
    when '-y'
      simple_prompt = true
    else
      puts "Unrecognized option: #{arg}"
    end
  end
end

def print_bytecodes(bc)
  str = ""
  bc.each do |b|
    str << b.chr
  end
  puts "\nBytecodes:", str.inspect
end

context = ""
line = 0
compiler = Bytecode::Compiler.new

# We use the same compiler state for each evaluation so we can save
# stuff like locals between statements.
compiler_state = RsLocalState.new

# Start off with just 10 slots, but we enlarge as needed below.
locals = Tuple.new(10)

pstate = ">"

while true
  if simple_prompt
    str = Readline.readline(">> ")
  else
    str = Readline.readline("sirb(eval):#{line.to_s.rjust(3, '0')}#{pstate} ")
  end
  break unless str
  context << "\n" unless context.empty?
  context << str
  line += 1
  if str.size == 0
    context = ""
  elsif context.size > 0
    begin
      # Enlarge locals as needed.
      if compiler_state.number_of_locals >= locals.fields
        nl = Tuple.new(compiler_state.locals.size + 10)
        nl.copy_from locals, 0
        locals = nl
      end
      
      if str == "local_variables\n"
        puts " Names: #{compiler_state.locals.inspect}\nValues: #{locals.inspect}\n"
        context = ""
      else  
        pstate = ">"
        sexp = context.to_sexp
        puts "\nS-exp:\n#{sexp.inspect}" if $show_parse
        nx = compiler.fully_normalize(sexp, compiler_state)
        puts "\nNormalized S-exp:\n#{nx.inspect}" if $show_sexp
        desc = compiler.compile_as_method(nx, :__eval_script__, compiler_state)
        puts "\nAssembly:\n#{desc.assembly}" if $show_asm
        cm = desc.to_cmethod
        print_bytecodes(cm.bytecodes) if $show_bytes
        out = cm.activate(MAIN, [], locals)
        
        # Support _ being the last value.
        locals.put 0, out
        
        puts "=> #{out.inspect}" # do it like this so exit won't do =>
        context = ""
        if $show_stats
          stats = VM.stats
          ch = stats.at(0)
          cm = stats.at(1)
          per = ch / cm
          puts "[Stats] [Cache] hits:#{ch}, misses:#{cm}, perc:#{per}"
        end
      end
    rescue SystemExit => e
      raise
    rescue Exception => e
      # Processing may continue with incomplete expressions
      unless SyntaxError === e and e.message =~ /unexpected \$end|unterminated string/
        puts ""
        puts "An exception has occurred:\n    #{e.message} (#{e.class})"
        puts "Backtrace:"
        puts e.backtrace.show
        context = ""
      else
        pstate = "*"
      end
    end
  else
    context = ""
  end
end
