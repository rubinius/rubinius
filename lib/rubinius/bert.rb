
require 'stringio'

require 'rubinius/bert/bert'
require 'rubinius/bert/types'

require 'rubinius/bert/decode'

require 'rubinius/bert/encode'

require 'rubinius/bert/encoder'
require 'rubinius/bert/decoder'

# Global method for specifying that an array should be encoded as a tuple.
def t
  BERT::Tuple
end
