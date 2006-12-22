require 'rubygems' 
require 'ruby2ruby'
require 'fileutils'
require 'test/unit'
require 'sydparse'

$:.unshift(File.dirname(__FILE__) + '/../lib')
require 'machine'

class RubiniusTargetError < RuntimeError; end

class RubiniusSpecExample; end

module RubiniusTarget
  def example(&block)
    raise ArgumentError, "you must pass a block" unless block_given?
    execute(compile(save_source(&block)))
  end
  
  def compile(source_name)
    `#{rubinius_path}/bin/rcompile #{source_name}`
    # Machine.new.compile_file(source, false)
    source_name + 'c'
  end
  
  def execute(compiled_file)
    r, w = IO.pipe
    r2, w2 = IO.pipe
    
    pid = fork {
      r.close
      w2.close
      STDOUT.reopen(w)
      STDIN.reopen(r2)
      Dir.chdir "#{rubinius_path}"
      
      exec "./shotgun/rubinius ./lib/kernel.rbc #{compiled_file}"
    }
    
    r2.close
    w.close
    w2.close
    Process.wait(pid)
    out = r.read
    status = $?.exitstatus
    if !status or status != 0
      if status == 1
        raise RubiniusTargetError, out << "    ========================================="
      elsif !status or status > 100
        raise RubiniusTargetError, "Shotgun has crashed"
      end
    end
    r.close
    out.chomp!
  end
  
  def template
    @template ||= <<-CODE
%s
RubiniusSpecExample.new.__example__
CODE
  end
  
  def save_source(&block)
    make_cache_directory
    RubiniusSpecExample.send(:define_method, :__example__, block)
    source = template % RubyToRuby.translate(RubiniusSpecExample)
    name = cache_source_name(source)
    File.open(name, "w") do |f|
      f << source
    end
    name
  end
  
  def cache_source_name(source)
    "#{cache_path}/#{caller_name}-#{source.hash.abs}.rb"
  end
  
  def caller_name(which=3)
    caller[which].match(/\/+(\w*).rb:(\d*)$/)[1..-1].join('-')
  end
  
  def rubinius_path
    File.expand_path(File.dirname(__FILE__) + '/..')
  end

  def cache_path
    rubinius_path + '/code-cache'
  end
  
  def make_cache_directory
    FileUtils.mkdir_p(cache_path) unless File.exists? cache_path
  end
end
