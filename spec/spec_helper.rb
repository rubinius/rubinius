require 'rubygems'
require 'test/unit'
require 'spec'
require 'fileutils'
$:.unshift(File.dirname(__FILE__) + '/../lib')

class RubiniusError < RuntimeError; end

class RubiniusTest < Test::Unit::TestCase
  def setup
    #setup_machine
    
    FileUtils.mkdir_p(cache_root) unless File.exists? cache_root
  end

  def teardown
  end

  def run(result)
  end
  
  def rubinius(code)
    execute(compile(code))
  end
  
  def execute(compiled_file)
    r, w = IO.pipe
    r2, w2 = IO.pipe
    
    pid = fork {
      r.close
      w2.close
      STDOUT.reopen(w)
      STDIN.reopen(r2)
      Dir.chdir "#{rubinius_root}"
      
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
        raise RubiniusError, out << "    ========================================="
      elsif !status or status > 100
        raise RubiniusError, "Shotgun has crashed"
      end
    end
    r.close
    out.chomp!
  end
  
  def compile(code)
    cache_file = "#{cache_root}/#{cache_name(code)}.rb"
    cache_compiled = cache_file + 'c'
    unless File.exists?(cache_file) and
           File.exists?(cache_compiled) and
           code == File.read(cache_file)
      File.open(cache_file, 'w') { |f| f << code }
      `#{rubinius_root}/bin/rcompile #{cache_file}`
    end
    cache_compiled
  end

  private
  def cache_name(code, which=2)
    caller[which].match(/\/+(\w*).rb:(\d*)$/)[1..-1].join('-') + "-#{code.hash.abs}"
  end
  
  def cache_root
    "#{rubinius_root}/code-cache"
  end

  def rubinius_root
    File.expand_path(File.dirname(__FILE__) + '/..')
  end  
end

module Kernel
  def context_with_rubinius(name, &block)
    context_without_rubinius name do
      inherit RubiniusTest
      self.class_eval &block
    end
  end
  
  alias context_without_rubinius context
  alias context context_with_rubinius
end
