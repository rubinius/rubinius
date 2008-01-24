require 'digest'
require 'ext/digest/sha1/sha1'

Digest.create :SHA1, 'rbx_Digest_SHA1_Init', 'rbx_Digest_SHA1_Update',
              'rbx_Digest_SHA1_Finish', (4 * 5 + 4 * 2 + 64), 64, 20

