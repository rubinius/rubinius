# Lists directories and files using
# recursion and the Pathname class.
# Submitted by Lukas Domagala

require 'pathname'

def recursive_dir(path)
  current_path = Pathname.new(path)
  current_files = []
  current_dirs = []
  current_path.children.each do |file|
    if file.directory?
      current_dirs << file
    else
      current_files << file
    end
  end

  current_dirs.each do |file|
    recursive_dir(file)
    @dirs << file.realpath
  end

  current_files.each do |file|
    @files << file.realpath
  end
end

Bench.run [100] do |n|
  n.times do
    @dirs = []
    @files = []
    recursive_dir("../")
    puts "Directories", "---", @dirs
    puts "Files", "---", @files
  end
end
