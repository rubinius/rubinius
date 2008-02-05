typedef OBJECT (*send_site_lookup)(STATE, OBJECT ss);

struct send_site {
  OBJECT name;
  OBJECT selector;

  send_site_lookup lookup;
};

#define SEND_SITE_OBJECT_FIELDS 2

void send_site_init(STATE);
OBJECT send_site_create(STATE, OBJECT name, send_site_lookup func);

