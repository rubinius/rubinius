module Rubinius
  RUBY_LIB_VERSION = 18

  def self.ruby18?
    RUBY_VERSION =~ /^1\.8/
  end

  def self.ruby19?
    RUBY_VERSION =~ /^1\.9/
  end

  def self.ruby20?
    RUBY_VERSION =~ /^2\.0/
  end

  class Compiler
    class Stage
    end

    class Writer
      attr_accessor :version
    end
  end

  class CompiledFile
    def self.dump(cm, file, signature=0, version=0)
      File.open(file, "wb") do |f|
        new("!RBIX", signature, version).encode_to(f, cm)
      end
    end
  end

  class Generator
    attr_accessor :post_args
  end

  class CompiledCode
    attr_accessor :post_args
  end

  module AST
    class Node
    end

    class FormalArguments < Node
      def post_args
        0
      end
    end

    class IterArguments < Node
      def post_args
        0
      end
    end
  end
end
