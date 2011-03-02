module KernelSpecs
  class A; end

  class B
    def to_str
      1
    end
  end

  class C
    def to_str
      "zam"
    end
  end

  def self.empty_binding
    binding
  end

  def self.objectifier
    yield Object.new
  end

  def self.cache_file(name, cache)
    code = File.read name

    var = eval(code, empty_binding, name) do |cm, be|
      Rubinius::CompiledFile.dump cm, cache
    end
  end

  def self.run_cache(cache)
    cl = Rubinius::CodeLoader.new cache
    cm = cl.load_compiled_file cache, Rubinius::Signature

    script = cm.create_script
    MAIN.__send__ :__script__
  end
end
