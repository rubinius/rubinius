module Rubinius
  class CodeDB
    attr_accessor :loaded_features
    attr_accessor :load_path

    def self.open(path)
      Rubinius.primitive :code_db_open
      raise PrimitiveFailure, "Rubinius::CodeDB.open primitive failed"
    end

    def self.load(m_id)
      Rubinius.primitive :code_db_load
      raise PrimitiveFailure, "Rubinius::CodeDB.load primitive failed"
    end

    def self.current
      @current
    end

    def self.load_feature(stem, ext, reload, record)
      current.load_feature stem, ext, reload, record
    end

    def self.store(code, stem, path, feature, record)
      current.store code, stem, path, feature, record
    end

    def close
      Rubinius.primitive :code_db_close
      raise PrimitiveFailure, "Rubinius::CodeDB#close primitive failed"
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
