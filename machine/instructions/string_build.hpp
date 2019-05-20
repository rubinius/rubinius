#include "instructions.hpp"

#include "class/string.hpp"

namespace rubinius {
  namespace instructions {
    inline bool string_build(STATE, CallFrame* call_frame, intptr_t count) {
      size_t size = 0;

      bool tainted = false;

      bool check_encoding = false;
      Encoding* enc = nil<Encoding>();

      // Figure out the total size
      for(int i = 0; i < count; i++) {
        Object* obj = stack_back(i);

        if(obj->reference_p()) {
          tainted |= obj->tainted_p();
        }

        String* str = try_as<String>(obj);

        if(str) {
          native_int cur_size = str->byte_size();
          native_int data_size = as<ByteArray>(str->data())->size();
          if(unlikely(cur_size > data_size)) {
            cur_size = data_size;
          }
          size += cur_size;
        } else {
          // This isn't how MRI does this. If sub isn't a String, it converts
          // the original object via any_to_s, not the bad value returned from #to_s.
          // This quite a bit harder to implement in rubinius atm, so I'm opting for
          // this way instead.

          str = obj->to_s(state, false);

          tainted |= str->tainted_p();
          native_int cur_size = str->byte_size();
          native_int data_size = as<ByteArray>(str->data())->size();
          if(unlikely(cur_size > data_size)) {
            cur_size = data_size;
          }
          size += cur_size;

          // TRICKY! Reuse the stack to store our new String value.
          stack_back(i) = str;
        }

        /*
         * TODO: Consider the case when -K is set (not implemented yet).
         */
        if(!check_encoding) {
          Encoding* str_enc = str->encoding(state);
          if(enc->nil_p()) {
            enc = str_enc;
          } else if(str_enc != enc) {
            check_encoding = true;
            enc = nil<Encoding>();
          }
        }
      }

      String* str = String::create(state, 0, size);
#ifdef RBX_ALLOC_TRACKING
      if(unlikely(state->vm()->allocation_tracking())) {
        str->setup_allocation_site(state);
      }
#endif

      uint8_t* pos = str->byte_address();
      native_int str_size = 0;

      for(int i = count - 1; i >= 0; i--) {
        Object* obj = stack_back(i);

        // We can force here because we've typed check them above.
        String* sub = force_as<String>(obj);

        native_int sub_size = sub->byte_size();
        native_int data_size = as<ByteArray>(sub->data())->size();
        if(unlikely(sub_size > data_size)) {
          sub_size = data_size;
        }

        if(check_encoding) {
          if(i < count - 1) {
            str->num_bytes(state, Fixnum::from(str_size));

            Encoding* enc = Encoding::compatible_p(state, str, sub);

            if(enc->nil_p()) {
              Exception::encoding_compatibility_error(state, str, sub);
              return false;
            } else {
              str->encoding(state, enc);
            }
          } else {
            str->encoding(state, sub->encoding());
          }
        }

        memcpy(pos + str_size, sub->byte_address(), sub_size);
        str_size += sub_size;
      }

      /* We had to set the size of the result String before every Encoding check
       * so we have to set it to the final size here.
       */
      if(check_encoding) {
        str->num_bytes(state, Fixnum::from(size));
        str->ascii_only(state, cNil);
      }
      if(!enc->nil_p()) str->encoding(state, enc);

      if(tainted) str->set_tainted();

      stack_clear(count);
      stack_push(str);
      return true;
    }
  }
}
