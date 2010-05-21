module BERT
  module Types
    SMALL_INT = 97
    INT = 98
    SMALL_BIGNUM = 110
    LARGE_BIGNUM = 111
    FLOAT = 99
    ATOM = 100
    SMALL_TUPLE = 104
    LARGE_TUPLE = 105
    NIL = 106
    STRING = 107
    LIST = 108
    BIN = 109
    FUN = 117
    NEW_FUN = 112
    MAGIC = 131
    MAX_INT = (1 << 27) -1
    MIN_INT = -(1 << 27)
  end
end