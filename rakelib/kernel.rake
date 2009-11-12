# All the tasks to manage the kernel

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

  puts "Compiling #{src}"
  Rubinius::CompilerNG.compile src, 1, rbc, :all
end

# Create file dependencies for all kernel files
if File.exists? "runtime/index"
  subdirs = IO.readlines("runtime/index").map { |line| line.chomp }
else
  raise "unable to build kernel, runtime/index is missing"
end

def kernel_dependency(name, files)
  rbc = name.sub(/^kernel/, "runtime") + "c"
  file rbc => [name, File.dirname(rbc)]
  files << rbc
end

kernel = ["runtime/platform.conf"]

subdirs.each do |subdir|
  directory "runtime/#{subdir}"

  Dir["kernel/#{subdir}/*.rb"].each do |rb|
    kernel_dependency rb, kernel
  end
end

opcodes = "lib/compiler/opcodes.rb"

FileList[ "kernel/bootstrap/rubinius_config.rb",
          "kernel/bootstrap/ruby_config.rb",
          "kernel/alpha.rb",
          "kernel/loader.rb"
].each do |rb|
  kernel_dependency rb, kernel
end

FileList[ "lib/compiler.rb",
          "lib/compiler/**/*.rb",
          opcodes,
          "lib/melbourne.rb",
          "lib/melbourne/**/*.rb"
].each do |rb|
  rbc = "#{rb}c"
  file rbc => rb
  kernel << rbc
end

namespace :compiler do
  task :load => [opcodes, "lib/ext/melbourne/ruby/melbourne.#{$dlext}"] do
    require File.expand_path("../../lib/compiler", __FILE__)
  end

  task :check => :load do
    unless ENV['NOCLEAN']
      existing = kernel.select { |name| name =~ /rbc$/ and File.exists? name }
      kernel_mtime = existing.map { |name| File.stat(name).mtime }.min

      compiler = (Dir["lib/compiler/**/*.rb", opcodes])
      compiler_mtime = compiler.map { |name| File.stat(name).mtime }.max

      kernel_clean if !kernel_mtime or compiler_mtime > kernel_mtime
    end
  end
end

desc "Build all kernel files"
task :kernel => 'kernel:build'

namespace :kernel do
  task :build => ['compiler:check'] + kernel do
    subdirs.each do |subdir|
      cp "kernel/#{subdir}/load_order.txt", "runtime/#{subdir}/load_order.txt"
    end
  end

  desc "Delete all .rbc files"
  task :clean do
    kernel_clean
  end
end
