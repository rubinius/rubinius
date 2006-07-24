require 'rubygems'
require 'inline'

class Memory
  
  class Fault < RuntimeError
  end
  
  inline do |builder|
    
    builder.include "<setjmp.h>"
    
    builder.prefix <<-CODE
    static int __segfault_occurred = 0;
    static sigjmp_buf __last_stack;
    
    #define check_segfault(reason) \
    if(sigsetjmp(__last_stack, 1)) \
      rb_raise(rb_path2class("Memory::Fault"), reason);
      
    #define on_segfault if(sigsetjmp(__last_stack, 1))
    
    void handle_segv(int sig, siginfo_t *b, void *c) {
      /*
      printf("Signal: %d\\n", b->si_signo);
      printf("Code: %d\\n", b->si_code);
      
      if(__segfault_occurred > 10) {
        abort();
      }
      __segfault_occurred++;
      */
      // printf("Segfault detected!\\n");
      siglongjmp(__last_stack, 1);
    }
    CODE
    
    builder.c_singleton <<-CODE
    void enable_protection() {
      stack_t sigstk;
      struct sigaction act;
      
      sigstk.ss_sp = (char*)malloc(SIGSTKSZ);
      sigstk.ss_size = SIGSTKSZ;
      sigstk.ss_flags = 0;
      sigaltstack(&sigstk, (stack_t*)0);
      memset(&act, 0, sizeof(struct sigaction));
      act.sa_sigaction = handle_segv;
      act.sa_flags = (SA_SIGINFO | SA_ONSTACK);
      
      sigaction(SIGSEGV, &act, NULL);
      sigaction(SIGBUS, &act, NULL);
      sigaction(SIGILL, &act, NULL);
      sigaction(SIGFPE, &act, NULL);
      
      // printf("Enabled memory protection.\\n");
    }
    CODE
    
    # Allocate +size+ bytes and return a pointer to the beginning
    builder.c_singleton <<-CODE
    unsigned long allocate_memory(unsigned long size) {
        void *ptr;
        if(!(ptr = malloc(size))) {
            return Qnil;
        }
        return (long)ptr;
    }
    CODE

    # Attempt to reallocate the memory located at +ptr+ to
    # +size+ bytes
    builder.c_singleton <<-CODE
    unsigned long resize_memory(unsigned long size, unsigned long ptr) {
        void *new, *old;
        old = (void*)ptr;
        on_segfault {
          rb_raise(rb_path2class("Memory::Fault"), "Unable to resize memory at 0x%X", ptr);
        }
        if(!(new = realloc(old, size))) {
            return Qnil;
        }

        return (long)new;
    }
    CODE

    # Release the memory allocated at +ptr+
    builder.c_singleton <<-CODE
    void release_memory(unsigned long ptr) {
      char *cptr;
      char test;
      char str[1];
      on_segfault {
        rb_raise(rb_path2class("Memory::Fault"), "Unable to free memory at 0x%X", ptr);
      }
      /* This code is here as a check for ptr. By referencing the data
       * and trying to use it, we'll generate a segfault if the address isn't
       * valid. I've found that most implementations of free() don't seem to
       * emit a segfault if the address was bad */
      cptr = (char*)ptr;
      test = *cptr;
      memcpy(str, &test, 1);
      free((void*)ptr);
    }
    CODE

    # Fetch a long +index+ longs from +intptr+
    builder.c_singleton <<-CODE
    long fetch_long(unsigned long intptr, unsigned long idx) {
        long *ptr = (long*)(intptr + (idx * sizeof(long)));
        long ret;
        on_segfault {
          rb_raise(rb_path2class("Memory::Fault"), "Unable to fetch long at 0x%X", intptr);
        }
        ret = (long)*ptr;
        return ret;
    }
    CODE

    # Store long +data+ at the memory address +index+ longs
    # from +intptr+
    builder.c_singleton <<-CODE
    void store_long(unsigned long intptr, unsigned long idx, long data) {
        long *ptr;
        on_segfault {
          rb_raise(rb_path2class("Memory::Fault"), "Unable to store long at 0x%X, index %d", intptr, idx);
        }
        ptr = (long*)(intptr + (idx * sizeof(long)));
        *ptr = data;
    }
    CODE
    
    # Fetch a byte +index+ bytes from +intptr+
    builder.c_singleton <<-CODE
    char fetch_byte(unsigned long intptr) {
        char *ptr = (char*)intptr;
        char ret;
        
        on_segfault {
          rb_raise(rb_path2class("Memory::Fault"), "Unable to fetch byte at 0x%X", intptr);
        }
        
        ret = (char)*ptr;
        return ret;
    }
    CODE

    # Store byte +data+ at the memory address +index+ bytes
    # from +intptr+
    builder.c_singleton <<-CODE
    void store_byte(unsigned long intptr, char data) {
        char *ptr = (char*)intptr;
        // *(ptr + (sizeof(char) * index)) = data;
        on_segfault {
          rb_raise(rb_path2class("Memory::Fault"), "Unable to store byte at 0x%X", intptr);
        }
        
        *ptr = data;
    }
    CODE
    
    # Useful because it lets the code know about how the hardware 
    # works.
    builder.c_singleton <<-CODE
    long pointer_size() {
      return sizeof(void*);
    }
    CODE
    
    # Returns the size of a long on this platform. Useful for upper
    # layers calculating things based on longs.
    builder.c_singleton <<-CODE
    long long_size() {
      return sizeof(long);
    }
    CODE
    
    # Copy +src_size+ bytes from address +src+ to address +dest+.
    builder.c_singleton <<-CODE
    void transfer_memory(unsigned long src, unsigned long src_size, 
        unsigned long dest) {
          on_segfault {
            rb_raise(rb_path2class("Memory::Fault"),
              "Unable to copy %d bytes from 0x%X to 0x%X", src_size, src, dest);
          }
            memcpy( (void*)dest, (void*)src, (size_t)src_size );
        }
    CODE
    
    # Clear +size+ bytes starting at address +ptr+.
    builder.c_singleton <<-CODE
    void clear_memory(unsigned long ptr, long size) {
      on_segfault {
        rb_raise(rb_path2class("Memory::Fault"), 
                    "Unable to clear memory at 0x%X, size %d", ptr, size);
      }
      memset((void*)ptr, 0, (size_t)size);
    }
    CODE
  end
end

# Enable segfault protection.
Memory.enable_protection