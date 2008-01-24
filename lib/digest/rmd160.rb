require 'digest'
require 'ext/digest/rmd160/rmd160'

Digest.create :RMD160, 'rbx_Digest_RMD160_Init', 'rbx_Digest_RMD160_Update',
              'rbx_Digest_RMD160_Finish', (4 * 5 + 4 * 2 + 64 + 4), 64, 20

