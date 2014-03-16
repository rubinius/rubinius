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

    c = Rubinius::ToolSets::Runtime::Compiler
    be = c.construct_block code, empty_binding, name

    cf = Rubinius::ToolSets::Runtime::CompiledFile
    cf.dump be.compiled_code, cache, 0, 0
  end

  def self.run_cache(cache)
    cl = Rubinius::CodeLoader.new cache
    cm = cl.load_compiled_file cache, 0, 0

    script = cm.create_script
    MAIN.__send__ :__script__
  end
end
