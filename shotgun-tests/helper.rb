class RubiniusError < RuntimeError; end

Signal.trap("CHLD") {}

module RubiniusHelper
  def caller_name(which=1)
    line = caller[which]
    parts = line.split(/\s+/)
    ln = parts.first.split(":")[-2]
    parts.last[1..-2]
  end
  
  def run_code(code, name=1, say=nil)
    name = caller_name(name) + "-#{code.hash}"
    path = "code-cache/#{name}.rb"
    if !File.exists?(path) or (code != File.read(path))    
      File.open(path, "w") do |fd|
        fd << code
      end
      system("./bin/rcompile code-cache/#{name}.rb")
    end
    r, w = IO.pipe
    r2, w2 = IO.pipe
    pid = fork {
      r.close
      w2.close
      STDOUT.reopen(w)
      STDIN.reopen(r2)
      exec "./shotgun/rubinius lib/kernel.rbc code-cache/#{name}.rbc"
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
  
end