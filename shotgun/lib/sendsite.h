#ifndef RBX_SENDSITE_H
#define RBX_SENDSITE_H


typedef struct send_site _send_site;

typedef void (*send_site_lookup)(struct message *msg);

struct send_site {
  /* OBJECT fields must be located at the start of the SendSite */
  OBJECT name;
  /* Reference to the CompiledMethod in which the SendSite is located */
  OBJECT sender;
  OBJECT selector;
  OBJECT data1;
  OBJECT data2;
  OBJECT data3;

  int data4;
  int hits, misses;
  send_site_lookup lookup;
  void *c_data;
};


#define SENDSITE(obj) ((struct send_site*)BYTES_OF(obj))

#define SEND_SITE_OBJECT_FIELDS 6

void send_site_init(STATE);
OBJECT send_site_create(STATE, OBJECT name);
void send_site_set_sender(STATE, OBJECT self, OBJECT cm);

#endif
