require 'rubygems'
require 'fileutils'
require 'test/unit'

class RubiniusError < RuntimeError; end

Signal.trap("CHLD") {}

class String
  def remove_head_indentation
    m = /^\s*/.match(self)
    return self unless m
    len = m[0].length
    gsub(/^\s{#{len}}/, '').strip
  end
end

module RubiniusHelper
  def assert_output(code, output)
    out = rp(code.remove_head_indentation)
    expected = output.remove_head_indentation.split(/\n/)
    assert_equal(out, expected)
  end

  def caller_name(which=1)
    line = caller[which]
    parts = line.split(/\s+/)
    ln = parts.first.split(":")[-2]
    parts.last[1..-2]
  end
  
  def run_code(code, name=1, say=nil)
    name = caller_name(name) + "-#{code.hash.abs}"
    path = "#{cache_root}/#{name}.rb"
    if !File.exists?(path) or !File.exists?(path + 'c') or (code != File.read(path))    
      File.open(path, "w") do |fd|
        fd << code
      end
      `#{rubinius_root}/bin/obsolete.rcompile #{cache_root}/#{name}.rb`
    end
    r, w = IO.pipe
    r2, w2 = IO.pipe
    
    cached_rbs_root, cached_cache_root = rubinius_root, cache_root
    pid = fork {
      r.close
      w2.close
      STDOUT.reopen(w)
      STDIN.reopen(r2)
      Dir.chdir "#{cached_cache_root}"
      ENV['RBX_BOOTSTRAP'] = "#{cached_rbs_root}/runtime/bootstrap.rba"
      ENV['RBX_CORE'] = "#{cached_rbs_root}/runtime/core.rba"
      exec "#{cached_rbs_root}/shotgun/rubinius #{cached_cache_root}/#{name}.rbc"
    }
    
    r2.close
    w.close
    if say
      w2 << say
    end
    w2.close
    Process.wait(pid)
    out = r.read
    status = $?.exitstatus
    if !status or status != 0
      error = out
      error << "\n    ========================================="
      if status == 1
        raise RubiniusError, error
      elsif !status or status > 100
        raise RubiniusError, "Shotgun has crashed"
      end
    end
    r.close
    return out
  end
  
  def rp(code)
    out = run_code(code, 2)
    out.strip.split("\n")
  end
  
  def ol(code)
    run_code("p #{code}\n", 2).strip
  end
  
  private
  def rubinius_root
    File.expand_path(File.dirname(__FILE__) + '/..')
  end
  
  def cache_root
    "#{rubinius_root}/code-cache"
  end
end

class RubiniusTestCase < Test::Unit::TestCase
  include RubiniusHelper
  undef_method :default_test
  
  def setup
    FileUtils.mkdir_p "#{cache_root}"
  end
end
