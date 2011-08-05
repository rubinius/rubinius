module Rubinius
  class EmbedLoader
    def self.load_compiler
      l = Loader.new
      l.preamble
      l.system_load_path
      CodeLoader.require_compiled "compiler"
    end
    
    def self.require_file file
      Kernel.require file
    end
    
    def self.eval_file file
      CodeLoader.load_script file
    end
    
    def self.eval_script code
      CodeLoader.execute_script code
    end
  end
end