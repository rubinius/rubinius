# (C) 2003-2005 Daniel J. Berger, All Rights Reserved
# License: Ruby's

require 'digest'

module Digest

  BUBBLEBABBLE_VOWELS     = %w/a e i o u y/
  BUBBLEBABBLE_CONSONANTS = %w/b c d f g h k l m n p r s t v z x/

  # Returns a babble form of the digest provided.  If +digest+ is not a
  # String, then .to_s is called on the argument first, which may alter
  # the result.

  def self.bubblebabble(digest)
    # Don't call to_s unless we have to since it alters the results
    if String === digest then
      dgst = digest.unpack("C*")
      dlen = digest.length
    else
      dgst = digest.to_s.unpack("C*")
      dlen = digest.to_s.length
    end

    seed   = 1
    rounds = dlen/2 + 1
    retval = 'x'

    0.upto(rounds - 1) { |i|
      if i+1 < rounds || (dlen % 2) > 0
        idx0 = (((dgst[2 * i] >> 6) & 3) + seed) % 6
        idx1 = (dgst[2 * i] >> 2) & 15
        idx2 = ((dgst[2 * i] & 3) + seed / 6) % 6
        retval << BUBBLEBABBLE_VOWELS[idx0] +
                  BUBBLEBABBLE_CONSONANTS[idx1] +
                  BUBBLEBABBLE_VOWELS[idx2]
        if i+1 < rounds
          idx3 = (dgst[2 * i + 1] >> 4) & 15
          idx4 = dgst[2 * i + 1] & 15;
          retval << BUBBLEBABBLE_CONSONANTS[idx3] + '-' +
                    BUBBLEBABBLE_CONSONANTS[idx4]
          seed = (seed * 5 + dgst[2 * i] * 7 + dgst[2 * i + 1]) % 36
        end
      else
        idx0 = seed % 6
        idx1 = 16
        idx2 = seed / 6
        retval << BUBBLEBABBLE_VOWELS[idx0] +
                  BUBBLEBABBLE_CONSONANTS[idx1] +
                  BUBBLEBABBLE_VOWELS[idx2]
      end
    }

    retval << 'x'

    return retval
  end

  class Instance
    def bubblebabble
      Digest.bubblebabble digest
    end
  end

  module Class
    def bubblebabble(string)
      data = digest string
      Digest.bubblebabble data
    end
  end

end

