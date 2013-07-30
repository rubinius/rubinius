---
layout: doc_en
title: Memory system
previous: Memory system
previous_url: memory-system
next: Garbage collector
next_url: memory-system/garbage-collector
---

## Introduction

The virtual machines knows about various types of objects. These objects
can be divided into different categories where each has its specific
properties. The first category is the so-called immediates, also known
as tagged pointers. This is a very efficient way of storing things like
smaller numbers and symbols.

Of course there are also the object types that the virtual machine knows
about and the general objects the virtual machine handles. They share
some generic properties, but also have their own characteristics such as
how they are layed out in memory and optimized.

## Immediates

This is the most simple and memory efficient type of objects, using only
a single word sized piece of memory. They are also known as tagged
pointers, because they use a simple trick that takes advantage of the
fact that pointers to Ruby objects in virtual machine are always aligned
and therefore not every possible word sized value is a valid pointer.

On for example a 32 bit system pointers are 32 bit aligned, so that means
that for example `0x1` can never be a valid pointer. This means that we
can use these address values for other purposes. The same of course is
also valid for 64 bit systems where pointers are aligned too.

We use the available space for a few different types, namely the
following ones:

1. __Fixnum__: We represent Fixnums with an immediate, where we use the tag
   `0x1`. This means that for example the value `1` is stored in memory as
   `0x3`. This means that operations on Fixnums have to remove the tag,
   run the operation and re-tag the resulting value. This is however
   much more efficient that to have to allocate memory for each `Fixnum` used.

1. __Symbol__: Symbols are represented by an immediate too and use the tag
   `0x6`. A separate symbol table stores the map from these immediate
   values to the corresponding string representation of the `Symbol`.

1. __True__, __False__, __Nil__ and __Undef__: These special literals are also
   presented this way. The bit patterns for these values are as follows:

       0x0a:false    1010
       0x1a:nil     11010
       0x12:true    10010
       0x22:undef  100010

   Undef is actually something only used in the Rubinius kernel in order
   to distinguish between having an optional argument provided or not,
   since using `nil` in that case would not allow for properly recognizing
   the case where `nil` is actually passed in. You might also see that the
   bit pattern for `false` and `nil` end in the same bits, this way we can just
   check that patter for a Ruby `false` match.

The use of these immediates means that we don't have to allocate any
memory for these types of objects. It also means that each instance of
for example the `Fixnum` value `1` is indistinguishable from other ones.

## Objects that really need memory

Besides immediates, we of course also have real objects that need memory
to be stored. Just writing your code with Fixnums and Symbols would get
quite tedious very fast.

Rubinius has a generic layout that applies to each of these actual
objects. The essence of this layout is as follows:

    +-------------------+
    | HeaderWord header |
    | Class* klass_     |
    | Object* ivars_    |
    +-------------------+

The `klass_` pointer points to the `Class` this object is an instance of. The
`ivars_` pointer points at a table that contains the instance variables
for this object. This is however not always used for all instance
variables, which can be read about in the section on [Packed object
layout].

This leaves us with the meaning of the first part, the `HeaderWord`. This
contains a 32 bit integer which consists of a number of flags and basic
properties about this object. It also has 32 bit of auxiliary data which
contains information depending on the meaning stored in the flags.

This is the order of the flags on a little endian architecture such as
x86. On big endian systems this order is reversed, which is necessary
since we use the meaning bits to mark if we have an inflated header or
not. What exactly an inflated header is, can be read in [the section on
inflated headers].

The first part shows the type and name of the flag, the last column is
the number of bits of the total flags header used for this flag.

{% highlight cpp %}
unsigned int inflated        : 1;
unsigned int meaning         : 2;
object_type  obj_type        : 8;
gc_zone      zone            : 2;
unsigned int age             : 4;

unsigned int Forwarded       : 1;
unsigned int Remember        : 1;
unsigned int Marked          : 2;

unsigned int InImmix         : 1;
unsigned int Pinned          : 1;

unsigned int Frozen          : 1;
unsigned int Tainted         : 1;
unsigned int Untrusted       : 1;
unsigned int LockContended   : 1;
unsigned int unused          : 5;
{% endhighlight %}

#### inflated

This header might actually not be a normal header but a pointer to an
inflated header. This bit is `1` when this is the case. It means that this
`1` bit should be masked and the header then be treated as a pointer to an
`InflatedHeader`.

InflatedHeaders are used when the regular header doesn't provide enough
room, such as the case when this object is used as a full mutex in
multiple threads.

#### meaning

The header might actually not be a regular header. There are 4 different
meanings defined:

1. __eAuxWordEmpty__: This is the default value and means that there's
   nothing special with this header.
1. __eAuxWordObjID__: This value means an object id was stored inside this
   header.
1. __eAuxWordLock__: This object is optimistically locked to a specific
   running thread. The auxiliary 32 bit of data contain the thread id
   that this object is locked for.
1. __eAuxWordHandle__: The `aux_word` contains an index to a C-API handle.
   This happens when this object is used in a C extension and needs a
   non moving handle to support MRI's C-API semantics.

#### obj\_type

This flag contains the object type and indicates the layout of this
object in memory. These types match the C++ types we have for the
objects in the virtual machine. This allows us using C++ templates
such as these for type safe operations:

{% highlight cpp %}
ByteArray ba = as<ByteArray>(obj);
{% endhighlight %}

In this case it validates that the given object is actually of the
correct `obj_type` and can throw an exception that bubbles back into Ruby
land so we don't crash the virtual machine but just get an exception.

#### zone

This flag indicates in which garbage collection zone these objects are
located. The value is one of the following:

1. __UnspecifiedZone__: Not yet in a zone. This only happens when we just
   allocated the object and haven't set the value. When debugging,
   normally you would never see this value.
1. __LargeObjectZone__: The object is a large object and therefore allocated
   in the large object zone.
1. __MatureObjectZone__: Object is mature, which means it was either
   allocated here or promoted to here from the young generation.
1. __YoungObjectZone__: Object is (almost) brand new and still young.

#### age

This is the age of the object. This tracks the number of young garbage
collection cycles that this object has survived. After a given number of
cycles the object is promoted to the mature generation.

#### Forwarded

Whether this is a forwarded object. Object forwarding is used when the
garbage collector has moved an object so other objects can updated their
references to it.

#### Remember

Set when we need to remember this object in the remember set of the
write barrier. For documentation on the write barrier, please see
[section on write barrier].

#### Marked

Mark used when garbage collecting. A rotating mark is used here, so
possible values are `0` for no mark or `1` or `2` depending on the rotation.

#### InImmix

Flag for whether this object is in the Immix space or not.

#### Pinned

A pinned object is not moved by the garbage collector. This is for
example used when reading data with the read system call which can block
and allows the garbage collector to run in the meanwhile. In this case
the buffer used to store the read data can't be moved.

#### Frozen

This represents the freezing concept also know in Ruby. This flag is set
when an object is frozen.

#### Tainted

Representation of the tainting of objects also available in Ruby.

#### Untrusted

Representation of the trust status of objects also available in Ruby.

#### LockContended

Flag used for marking whether threads are contending for locking this
object. When this happens, the object is inflated and the actual mutex
is stored in the inflated header.

## Virtual machine object types

The virtual machine knows about a number of object types in order to be
able to work with them. These objects also have a corresponding C++ class
that mimics the Ruby class and makes using them in C++ very easy and almost
transparent. As can be read in the previous section the `obj_type` flag
indicates this type.

These objects are also C++ objects and inherit from the C++ type `Object`
which in its turn inherits from `ObjectHeader`. Instance variables that
are defined in the C++ class can be made available in Ruby very easily
as can be seen in the following code snippet that can be found in these
C++ classes:

{% highlight cpp %}
namespace rubinius {
  class String : public Object {
  private:
    Fixnum* hash_value_;       // slot
  public:
    attr_accessor(hash_value, Fixnum);
  }
}
{% endhighlight %}

In this case you see here the `String` class defined which has a
`hash_value_` instance variable. This instance variable is made
available with accessors that work from both C++, but also are used
when the `@hash_value` instance variable in this case is set in Ruby.
Please note that this doesn't setup accessor methods in Ruby itself.
This means that setting an instance variable from Ruby goes through this
accessor in C++. If this instance variable should also be exposed in
Ruby, you still need to add an `attr_accessor` in Ruby in the given class.

This mapping between the types that the virtual machine is aware of
makes it a very easy mental model when switching from C++ to Ruby code
when working on Rubinius.

## Generic object layout

Besides these types that the virtual machine knows about, there are of
course also your every day regular Ruby objects that everyone uses in
their applications. These generic objects of course also have to store
instance variables, just like the object types the virtual machine is
aware of.

As you might have seen earlier in this part of the documentation, the
generic object layout also includes an `Object` pointer called `ivars`. As
you might already thought, this is a pointer to a so called `LookupTable`
that contains the instance variables for this object. A `LookupTable` is a
structure much like Ruby's `Hash`, but it uses only Symbols as the keys.

## Packed object layout

Since Ruby is a very flexible language, you never know up front what the
instance variables will be that people will use in their objects. This
is the reason that we need the `LookupTable` as described in the previous
section.

Even though Ruby is very dynamic, a lot of code that is executed
actually is pretty static. You almost never create all kinds of dynamic
instance variables at runtime and this set is pretty static. We can
actually use this property to optimize this both in terms of memory
usage and speed.

What the Rubinius compiler actually does is that it tracks all the
instance variables it sees for a `Module` or `Class` during compilation.
Each of those variables it encounters is stored in the given `Module` or
`Class`, as can be seen in this code snippet.

{% highlight ruby %}
class Foo
  def initialize
    @str = "awesome"
  end
end

Foo.instance_variable_get("@seen_ivars") => [:@str]
{% endhighlight %}

As you can see here, the compiler has tracked the compilation and has
seen the `@str` instance variable. The virtual machine can then use this
information to create an optimized object layout for the class `Foo`. This
tracking also works for modules that are included into classes. In that
case the instance variables tracked for the module are added to the
class it is included into.

The first time a new instance of `Foo` is created, the virtual machine
creates a specific layout for `Foo`. In this layout, room is reserved for
the seen instance variables. If you would write this out in a C++
object, it would look something like this:

{% highlight cpp %}
class Foo : public Object {
  Object* str;
}
{% endhighlight %}

This so-called packing of instance variables has multiple advantages.
One is that this creates more memory efficient objects. We don't have
the overhead of a LookupTable for storing them, so we use less memory
when we make objects like this.

Another advantage is that using this also results in faster code.
Retrieving or setting an instance variable doesn't incur a `LookupTable`
lookup anymore. The location of where the instance variable is stored
can be determined with just the address of the object and an offset
because this offset is always the same for all `Foo` instances.

Of course it is still possible to set an instance variable on an
instance of `Foo` after the first instance has been created. Maybe it's
happening due to reopening the class later on after the first `Foo` is
instantiated or we're doing some metaprogramming. In that case we use
the `ivars` `LookupTable` as the fallback mechanism so we can keep providing
the flexibility that Ruby gives you.
