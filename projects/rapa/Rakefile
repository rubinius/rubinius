DIR       = File.expand_path "../", __FILE__
if dir = ENV["DIR"]
  OUT_DIR = File.expand_path ENV["DIR"]
else
  OUT_DIR = File.expand_path "../", __FILE__
end
MACHINES  = DIR + "/machines"
RUBINIUS  = DIR + "/actions/rubinius"
RUBINIUS_RAGEL = "ragel -C -G1 -I #{MACHINES} -I #{RUBINIUS}"

# The Ragel generated line info is a mess. This just strips the
# info so that any compile errors are reported directly from the
# generated code. This may be confusing. Other solutions welcome.
# BTW, the -L option just puts the #line directives into comments,
# which just clutters the generated code needlessly.
def remove_line_references(name, file)
  source = IO.readlines name
  source.reject! { |line| line =~ /^#line\s(\d+)\s"[^"]+"/ }
  File.open(name, "w") { |f| f.puts source }
end

namespace :build do
  desc "Generate Array#pack, String#unpack primitives for Rubinius"
  task :rbx => ["rbx:pack", "rbx:unpack"]

  namespace :rbx do
    task :pack do
      input  = "#{RUBINIUS}/pack_code.rl"
      output = "#{OUT_DIR}/pack.cpp"

      sh "#{RUBINIUS_RAGEL} -o #{output} #{input}"
      remove_line_references output, "vm/builtin/pack.cpp"
    end

    task :unpack do
      input  = "#{RUBINIUS}/unpack_code.rl"
      output = "#{OUT_DIR}/unpack.cpp"

      sh "#{RUBINIUS_RAGEL} -o #{output} #{input}"
      remove_line_references output, "vm/builtin/unpack.cpp"
    end
  end
end
