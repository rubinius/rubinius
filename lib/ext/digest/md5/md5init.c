#include "md5.h"
#include "ruby.h"

MD5_CTX *rbx_Digest_MD5_Alloc() {
  return (MD5_CTX *)malloc(sizeof(MD5_CTX));
}

int rbx_Digest_MD5_StructSize(){
	return sizeof(MD5_CTX);
}

Init_md5()
{
}
