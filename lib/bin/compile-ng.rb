require "compiler-ng"

class CompilerScript
  def options(argv=ARGV)
    options = Rubinius::Options.new "Usage: rbx compile-ng [options] [files]", 25

    options.on "-o", "--output NAME", "Compile single input file to NAME" do |n|
      @output_name = n
    end

    options.help

    @sources = options.parse argv
  end

  def collect_files
    @files = []

    @sources.each do |entry|
      if File.directory? entry
        spec = "#{entry}/**/*.rb"
      else
        spec = entry
      end

      @files += Dir[spec]
    end
  end

  def run
    @files.each do |file|
      Rubinius::CompilerNG.compile file
    end
  end

  def main
    options
    collect_files
    run
  end
end

CompilerScript.new.main
