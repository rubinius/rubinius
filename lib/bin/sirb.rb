#!/usr/bin/env rbx

require 'bytecode/compiler'
require 'bytecode/rubinius'

# recognize a few simple options
def usage
  puts <<-USAGE
Usage: sirb [options]
  sirb is a RCEPL (read, compile, execute, print, loop) program for rubinius

  Options:
          -b   Print bytecode encoding
          -h   This message
          -p   Print the parsed s-expression
          -s   Print assembly instructions
          -t   Print stats
          -y   Simple prompt
  USAGE
end

env = ENV['SIRB_ARGS']
if env
  env.split(" ").each do |part|
    ARGV << part
  end
end

simple_prompt = false

until ARGV.empty?
  arg = ARGV.shift
  case arg
  when '-b'
    $show_bytes = true
  when '-h'
    usage
    exit
  when '-p'
    $show_parse = true
  when '-s'
    $show_asm = true
  when '-t'
    $show_stats = true
  when '-y'
    simple_prompt = true
  else
    puts "Unrecognized option: #{arg}"
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
compiler_state.uses_eval = true # Don't allocate locals on the stack

# Start off with just 10 slots, but we enlarge as needed below.
locals = Tuple.new(10)

# Alias quit to exit to match IRB, make it easier to quit
class Object; alias :quit :exit; end

pstate = ">"

while true
  prompt = if simple_prompt then
             ">#{pstate} "
           else
             "sirb(eval):#{line.to_s.rjust(3, '0')}#{pstate} "
           end

  begin
    str = Readline.readline prompt
  rescue Interrupt
    next
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
        nl = Tuple.new(compiler_state.number_of_locals + 10)
        nl.copy_from locals, 0
        locals = nl
      end
     
      if str == "local_variables"
        h = Hash.new
        compiler_state.locals.keys.each_with_index do |k,i|
          h[k] = locals.at(i)
        end
        puts h.inspect
        context = ""
      else  
        pstate = ">"
        sexp = context.to_sexp
        puts "\nS-exp:\n#{sexp.inspect}" if $show_parse
        desc = compiler.compile_as_method(sexp, :__eval_script__, compiler_state)
        puts "\nAssembly:\n#{desc.assembly}" if $show_asm
        cm = desc.to_cmethod
        cm.compile
        print_bytecodes(cm.bytecodes) if $show_bytes
        out = cm.activate(MAIN, Object, [], locals)
        
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
