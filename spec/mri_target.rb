require 'test/unit'
require 'ruby2ruby'
require 'fileutils'

class MRITargetError < RuntimeError; end

class RubiniusSpecExample; end

module MRITarget
  def example(src='', &block)
    raise ArgumentError, "you must pass a block" unless block_given?
    execute(source(src, &block))
  end
  
  def source(src, &block)
    make_cache_directory
    RubiniusSpecExample.send(:define_method, :__example__, block)
    source = template % [src, RubyToRuby.translate(RubiniusSpecExample)]
    name = cache_source_name(source)
    unless File.exists?(name) and source == File.read(name)
      File.open(name, "w") { |f| f << source }
    end
    name
  end
  
  def execute(name)
    r, w = IO.pipe
    r2, w2 = IO.pipe
    
    pid = fork {
      r.close
      w2.close
      STDOUT.reopen(w)
      STDIN.reopen(r2)
      Dir.chdir "#{rubinius_path}"
      
      exec "ruby #{name}"
    }
    
    r2.close
    w.close
    w2.close
    Process.wait(pid)
    out = r.read
    status = $?.exitstatus
    if !status or status != 0
      raise MRITargetError, out
    end
    r.close
    out.chomp!
  end

  def template
    @template ||= <<-CODE
%s
%s
RubiniusSpecExample.new.__example__
CODE
  end

  def cache_source_name(source)
    "#{cache_path}/#{caller_name}-#{source.hash.abs}-mri.rb"
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
