module Rubinius
  class CodeDB
    class << self
      undef :new
    end

    def self.open(path)
      Rubinius.primitive :code_db_open
      raise PrimitiveFailure, "Rubinius::CodeDB.open primitive failed"
    end

    def self.load(m_id)
      Rubinius.primitive :code_db_load
      raise PrimitiveFailure, "Rubinius::CodeDB.load primitive failed"
    end

    def store(code)
      Rubinius.primitive :code_db_store
      raise PrimitiveFailure, "Rubinius::CodeDB#store primitive failed"
    end
  end
end
