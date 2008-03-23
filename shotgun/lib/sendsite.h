#ifndef RBX_SENDSITE_H
#define RBX_SENDSITE_H


typedef struct send_site _send_site;

typedef void (*send_site_lookup)(struct message *msg);

/* 

 Send sites is the core of method dispatch caching used by Rubinius.
 It is half C, half Ruby implementation. It is created for every message
 send site (or, for those coming from Java world, method call site) to
 store information about receiver and method responding to message
 sent. This information later help use cached compiled method if the method
 receiver handles message with has already been executed at this send site.

 Selector is an object that represents a message (method name) and collection
 of references to every send site that uses the same message. Selectors are
 not used in method dispatch process though: they provide a reverse lookup
 of send sites for given method name.

 You can pass -ps and -pss flags to Rubinius on launch and summary of most
 frequently encountered selectors and send sites will be printed on exit
 (so they stand for "print selectors" and "print send sites").
 Note that it summarizes most often send messages (method names), not actual methods
 executed, because many methods may have the same name in different classes or
 modules.

 Note that OBJECT fields must be located at the start of the SendSite.


 == References

 Send sites are also known as call sites in a variery of literature and on
 Wikipedia:
 http://tinyurl.com/22jpuy

 Adam Gardiner wrote a great post on send sites in Rubinius:
 http://tinyurl.com/34c6e8


 == Structure fields

 name     : the name of the message (i.e. method) this send site sends (calls)
 sender   : reference to the CompiledMethod in which the SendSite is located
 selector : a reference to the Selector instance corresponding to the message name
 data1    : class of the receiver
 data2    : The CompiledMethod corresponding to this message on the receiver class,
            as encountered on the last dispatch. When a message is dispatched,
            this is the target object that needs to be located; it contains the
            bytecode for the method on the receiver.
 data3    : the module
 data4    : the primitive index if the SendSite resolves to a primitive method
 hits     : counter of successfull caches of the receiver method
 misses   : counter of unsuccessful caches, respectively
 lookup   : pointer to function that performs actual method lookup
 c_data   : for an FFI send site, holds the address of the FFI stub function to call.
            for a primitive send site, holds the address of the primitive function
            to call.
*/
struct send_site {
  OBJECT name;
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
