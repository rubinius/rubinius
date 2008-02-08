
typedef struct send_site _send_site;

typedef int (*send_site_lookup)(STATE, cpu c, _send_site *ss, struct message *msg);

struct send_site {
  OBJECT name;
  OBJECT selector;
  OBJECT data1;
  OBJECT data2;
  OBJECT data3;

  send_site_lookup lookup;
  void *c_data;
};


#define SENDSITE(obj) ((struct send_site*)(obj->field))

#define SEND_SITE_OBJECT_FIELDS 5

#define SEND_SITE_ABORT 0
#define SEND_SITE_DONE 1
#define SEND_SITE_RESOLVED 2
#define SEND_SITE_BYTECODE 3

void send_site_init(STATE);
OBJECT send_site_create(STATE, OBJECT name, send_site_lookup func);

