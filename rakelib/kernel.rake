# All the tasks to manage the kernel

def compile_ruby(src, rbc, check_mtime = false, kernel = false)
  if check_mtime and File.readable?(rbc)
    return if File.mtime(rbc) >= File.mtime(src)
  end

  dir = File.dirname rbc
  FileUtils.mkdir_p dir unless File.directory? dir

  flags = kernel ? ["rbx-safe-math", "rbx-kernel"] : []
  mri_compile src, rbc, false, flags
end

# drake does not allow invoke to be called inside tasks
def kernel_clean
  rm_f Dir["**/*.rbc",
           "**/.*.rbc",
           "spec/capi/ext/*.{o,sig,#{$dlext}}",
           "runtime/**/load_order.txt",
           "runtime/platform.conf"],
    :verbose => $verbose
end

# The default rule for compiling all kernel files use the
# rbx-kernel compiler flag. Other files are exception cases.
rule ".rbc" do |t|
  rbc = t.name
  src = t.prerequisites.first

  compile_ruby src, rbc, false, true
end

# Create file dependencies for all kernel files
unless File.exists? "runtime/index"
  raise "unable to build kernel, runtime/index is missing"
else
  subdirs = IO.readlines("runtime/index").map { |line| line.chomp }
end

kernel = ["runtime/platform.conf"]

subdirs.each do |subdir|
  Dir["kernel/#{subdir}/*.rb"].each do |rb|
    rbc = "runtime/#{subdir}/#{File.basename(rb)}c"
    kernel << rbc
    file rbc => rb
  end
end

rb  = "kernel/bootstrap/rubinius_config.rb"
rbc = "runtime/bootstrap/rubinius_config.rbc"
file rbc => rb
kernel << rb << rbc

rb  = "kernel/bootstrap/ruby_config.rb"
rbc = "runtime/bootstrap/ruby_config.rbc"
file rbc => rb
kernel << rb << rbc

file "runtime/alpha.rbc" => "kernel/alpha.rb"
kernel << "runtime/alpha.rbc"

opcodes = "kernel/compiler/opcodes.rb"
file "runtime/compiler/opcodes.rbc" => opcodes
kernel << "runtime/compiler/opcodes.rbc"

file "runtime/loader.rbc" => "kernel/loader.rb" do |t|
  rbc = t.name
  src = t.prerequisites.first

  compile_ruby src, rbc, false, true
end
kernel << "runtime/loader.rbc"

# TODO: extra_compiler is temporary until sexp processing is removed
# from the compiler.
extra_compiler = ["lib/strscan.rb",
                  "lib/stringio.rb",
                  "lib/racc/parser.rb",
                  "lib/ruby_lexer.rb",
                  "lib/ruby_parser.rb",
                  "lib/ruby_parser_extras.rb",
                  "lib/sexp_processor.rb"]
extra_compiler.each do |rb|
  rbc = "#{rb}c"
  kernel << rbc

  file rbc => rb do |t|
    src = t.prerequisites.first
    dst = t.name

    compile_ruby src, dst
  end
end

desc "Build all kernel files"
task :kernel => 'kernel:build'

namespace :kernel do
  task :load_compiler => opcodes do
    require 'lib/compiler/mri_compile'
  end

  task :check_compiler => :load_compiler do
    unless ENV['NOCLEAN']
      existing = kernel.select { |name| name =~ /rbc$/ and File.exists? name }
      kernel_mtime = existing.map { |name| File.stat(name).mtime }.min

      compiler = (Dir["kernel/compiler/*.rb"] + extra_compiler)
      compiler_mtime = compiler.map { |name| File.stat(name).mtime }.max

      kernel_clean if !kernel_mtime or compiler_mtime > kernel_mtime
    end
  end

  task :build => ['kernel:check_compiler'] + kernel + extra_compiler do
    subdirs.each do |subdir|
      cp "kernel/#{subdir}/load_order.txt", "runtime/#{subdir}/load_order.txt"
    end
  end

  desc "Delete all .rbc files"
  task :clean do
    kernel_clean
  end

  desc "Create dot(1) files of dependencies in bootstrap, common, delta"
  task :graph do
    begin
      $: << File.expand_path("~/Work/p4/zss/src/ZenHacks/dev/lib/")
      require 'graph'
    rescue LoadError
      abort 'install ZenHacks'
    end

    %w[bootstrap common delta].each do |path|
      graph = Graph.new
      graph.prefix << 'rankdir="LR"'

      Dir[File.join('kernel', path, '*.rb')].each do |file|
        depends = nil
        open file do |io| depends = io.gets end

        if depends =~ /^# depends on: (.*)/ then
          $1.split.each do |depend|
            graph[file] << File.join('kernel', path, depend)
          end
        else
          graph[file] << 'Nothing'
        end

        open "#{path}_depends.dot", 'w' do |io|
          io << graph
        end
      end
    end
  end

end
