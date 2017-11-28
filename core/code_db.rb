module Rubinius
  class CodeDB
    attr_accessor :loaded_features
    attr_accessor :load_path

    class << self
      # CodeDB.open is the constructor
      private :new
    end

    def self.open(path)
      Rubinius.primitive :code_db_open
      raise PrimitiveFailure, "Rubinius::CodeDB.open primitive failed"
    end

    def load_db
      Rubinius.primitive :code_db_load_db
      raise PrimitiveFailure, "Rubinius::CodeDB#load_db primitive failed"
    end

    def close
      Rubinius.primitive :code_db_close
      raise PrimitiveFailure, "Rubinius::CodeDB#close primitive failed"
    end

    def load(id)
      Rubinius.primitive :code_db_load
      raise PrimitiveFailure, "Rubinius::CodeDB#load primitive failed"
    end

    def load_feature(stem, ext, reload, record)
      Rubinius.primitive :code_db_load_feature
      raise PrimitiveFailure, "Rubinius::CodeDB#load_feature primitive failed"
    end

    def store(code, stem, path, feature, record)
      Rubinius.primitive :code_db_store
      raise PrimitiveFailure, "Rubinius::CodeDB#store primitive failed"
    end
  end
end
