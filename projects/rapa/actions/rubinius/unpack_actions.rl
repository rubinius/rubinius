// vim: filetype=ragel

%%{

  machine unpack;

  action start {
    count = 1;
    rest = false;
    platform = false;
  }

  action start_digit {
    count = fc - '0';
  }

  action count {
    count = count * 10 + (fc - '0');
  }

  action rest {
    count = 0;
    rest = true;
  }

  action zero_count {
    count = 0;
  }

  action platform {
    platform = true;
  }

  action byte_width {
    width = 1;
  }

  action short_width {
    width = 2;
  }

  action int_width {
    width = 4;
  }

  action long_width {
    width = 8;
  }

  action platform_width {
    if(platform) {
#if RBX_SIZEOF_LONG == 4
      width = 4;
#else
      width = 8;
#endif
    } else {
      width = 4;
    }
  }

  action set_stop {
    stop = rest ? bytes_size + 1 : index + width * count;
    if(stop > bytes_size) {
      stop = index + ((bytes_size - index) / width) * width;
    }
  }

  action extra {
    for(; count > 0; count--) {
      array->append(state, Qnil);
    }
  }

  # Integers

  action C {
    unpack_integer(ubyte);
  }

  action c {
    unpack_integer(sbyte);
  }

  action S {
    unpack_integer(u2bytes);
  }

  action s {
    unpack_integer(s2bytes);
  }

  action I {
    unpack_integer(u4bytes);
  }

  action i {
    unpack_integer(s4bytes);
  }

  action L {
    if(platform) {
#if RBX_SIZEOF_LONG == 4
      unpack_integer(u4bytes);
#else
      unpack_integer(u8bytes);
#endif
    } else {
      unpack_integer(u4bytes);
    }
  }

  action l {
    if(platform) {
#if RBX_SIZEOF_LONG == 4
      unpack_integer(s4bytes);
#else
      unpack_integer(s8bytes);
#endif
    } else {
      unpack_integer(s4bytes);
    }
  }

  action N {
    unpack_integer(u4bytes_be);
  }

  action n {
    unpack_integer(u2bytes_be);
  }

  action V {
    unpack_integer(u4bytes_le);
  }

  action v {
    unpack_integer(u2bytes_le);
  }

  action Q {
    unpack_integer(u8bytes);
  }

  action q {
    unpack_integer(s8bytes);
  }

  # Floats

  action D {
    unpack_double;
  }

  action E {
    unpack_double_le;
  }

  action e {
    unpack_float_le;
  }

  action F {
    unpack_float;
  }

  action G {
    unpack_double_be;
  }

  action g {
    unpack_float_be;
  }

  # Moves

  action X {
    if(rest) count = bytes_size - index;
    index -= count;
  }

  action x {
    if(rest) {
      index = bytes_size;
    } else {
      index += count;
    }
  }

  action at {
    if(!rest) {
      index = count;
    }
  }

  action check_bounds {
    if(index < 0 || index > bytes_size) {
      std::ostringstream msg;
      msg << *p << " outside of string";
      Exception::argument_error(state, msg.str().c_str());
    }
  }

  # String / Encoding helpers

  action bytes {
    bytes = (const char*)self->byte_address() + index;
  }

  action bytes_end {
    bytes_end = (const char*)self->byte_address() + bytes_size;
  }

  action string_width {
    width = 1;
  }

  action bit_width {
    width = 8;
  }

  action hex_width {
    width = 2;
  }

  action remainder {
    remainder = bytes_size - index;
  }

  action rest_count {
    if(rest) {
      count = remainder;
    } else if(count > remainder) {
      count = remainder;
    }
  }

  action string_size {
    if(rest || count > remainder * width) {
      count = remainder * width;
    }
  }

  # Strings

  action A {
    String* string;

    if(count > 0) {
      native_int i;
      for(i = count; i > 0; i--) {
        uint8_t c = bytes[i-1];
        if(c != ' ' && c != '\0')
          break;
      }
      string = String::create(state, bytes, i);
    } else {
      string = String::create(state, "");
    }

    array->append(state, string);
    unpack::increment(index, count, bytes_size);
  }

  action a {
    array->append(state, String::create(state, bytes, count));

    unpack::increment(index, count, bytes_size);
  }

  action Z {
    native_int c;
    for(c = 0; c < count; c++) {
      if(bytes[c] == '\0') break;
    }
    array->append(state, String::create(state, bytes, c));

    if(rest) {
      unpack::increment(index, c < count ? c + 1 : count, bytes_size);
    } else {
      unpack::increment(index, count, bytes_size);
    }
  }

  # Encodings

  action index_increment {
    unpack::increment(index,
                      bytes - ((const char*)self->byte_address() + index),
                      bytes_size);
  }

  action B {
    array->append(state, unpack::bit_high(state, bytes, count));
  }

  action b {
    array->append(state, unpack::bit_low(state, bytes, count));
  }

  action H {
    array->append(state, unpack::hex_high(state, bytes, count));
  }

  action h {
    array->append(state, unpack::hex_low(state, bytes, count));
  }

  action M {
    array->append(state, unpack::quotable_printable(state, bytes, bytes_end, remainder));
  }

  action m {
    array->append(state, unpack::base64_decode(state, bytes, bytes_end, remainder));
  }

  action U {
    unpack::utf8_decode(state, array, bytes, bytes_end, count, index);
  }

  action u {
    array->append(state, unpack::uu_decode(state, bytes, bytes_end, remainder));
  }

  action w {
    unpack::ber_decode(state, array, bytes, bytes_end, count, index);
    index = bytes - (const char*)self->byte_address();
  }

  action non_native_error {
    std::ostringstream msg;
    msg << "'" << *p << "' allowed only after types sSiIlL";
    Exception::argument_error(state, msg.str().c_str());
  }

  action done {
    return array;
  }
}%%
