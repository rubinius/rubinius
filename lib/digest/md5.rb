require 'digest'
require 'ext/digest/md5/md5'

Digest.create :MD5, 'rbx_Digest_MD5_Init', 'rbx_Digest_MD5_Update',
              'rbx_Digest_MD5_Finish', (4 * 2 + 4 * 4 + 64), 64, 16

