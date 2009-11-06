require 'pp'

unless defined?(RUBY_ENGINE) and RUBY_ENGINE == 'rbx'
  $: << 'lib'
  require File.join(File.dirname(__FILE__), '..', 'compiler', 'mri_shim')
end

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

    pp code
    puts ""
    puts c.into_script(code).to_description.generator.text
    puts ""
  end

  exit
end

def describe_compiled_method(cm, dis=false)
  extra = cm.literals.to_a.find_all { |l| l.kind_of? Rubinius::CompiledMethod }

  name = cm.name ? cm.name.inspect : 'anonymous'
  markers = (36 - name.size) / 2
  heading = "#{'=' * markers} #{name} #{'=' * (markers + name.size % 2)}"
  puts heading
  puts "contains #{extra.size} CompiledMethods" unless extra.empty?
  puts "object_id: 0x#{cm.object_id.to_s(16)}"
  puts "total args: #{cm.total_args} required: #{cm.required_args}"
  print " (splatted)" if cm.splat
  puts "stack size: #{cm.stack_size}, local count: #{cm.local_count}"
  puts "lines: #{cm.lines.inspect}"
  puts ""
  puts cm.decode
  puts "-" * 38

  if dis
    mm = cm.make_machine_method
    puts "\nx86 Assembly:"
    mm.disassemble
    puts
  end

  until extra.empty?
    puts ""
    sub = extra.shift
    describe_compiled_method(sub, dis)
  end

end

# Temporary workaround for Rubinius bug in __FILE__ paths
if __FILE__.include?($0) then
  flags = []
  file = nil

  while arg = ARGV.shift
    case arg
    when /-I(.+)/ then
      other_paths = $1[2..-1].split(":")
      other_paths.each { |n| $:.unshift n }
    when /-f(.+)/ then
      flags << $1
    else
      file = arg
      break
    end
  end

  unless file
    interactive()
    exit 0
  end

  begin
    puts "Enabled flags: #{flags.join(', ')}" unless flags.empty?
    puts "File: #{file}"

    puts "Sexp:"
    pp File.to_sexp(file)

    puts "\nCompiled output:"
    top = Rubinius::CompilerNG.compile_file_old(file, flags)
    describe_compiled_method(top, flags.include?("dis"))

  rescue SyntaxError
    exit 1
  end
end
