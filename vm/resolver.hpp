#ifndef RBX_RESOLVER_HPP
#define RBX_RESOLVER_HPP

namespace rubinius {

  class Executable;


  /**
   *  MethodResolvers handle locating the correct method to execute.
   *
   *  Each SendSite in the program uses its own MethodResolver, which
   *  allows various optimizations mainly in caching. The resolver can
   *  be changed at any time so that optimized paths can be set up as
   *  well as removed as necessary.
   *
   *  MethodResolver is an interface class (an abstract base class
   *  to be precise) so the used resolvers will always be subclasses.
   *
   *  Implementations are located in vm/builtin/sendsite.cpp.
   *
   */
  class MethodResolver {

  public:   /* Ctors */

    virtual ~MethodResolver() { }

  public:   /* Interface */

    /**
     *  Locate an Executable for a method call.
     *
     *  Given a Message object, locate the actual Executable object
     *  to be called, the Module in which it was found and store it
     *  in the Message.
     *
     *  Pure virtual method. Implementations in subclasses only.
     */
    virtual bool resolve(STATE, Message& msg) = 0;
  };


  /**
   *  Standard Ruby method lookup implementation.
   *
   *  The normal, slow method lookup path up the inheritance chain.
   */
  class HierarchyResolver : public MethodResolver
  {
  public:   /* Ctors */

    virtual ~HierarchyResolver() { }

  public:   /* Interface */

    /**
     *  Standard Ruby method lookup path.
     *
     *  The normal, slow method lookup path. Given the Module in which
     *  to start looking and the method name, the lookup progresses up
     *  the inheritance tree until the method is located or no more
     *  method tables exist. If the method was found, it is verified
     *  that the method is visible (see MethodVisibility) for this
     *  call. If everything checks out, the Executable object representing
     *  the method and the Module in which it was found are stored in
     *  the Message.
     *
     *  @returns  true if method is found, false otherwise.
     */
    virtual bool resolve(STATE, Message& msg);
  };

  /**
   *  Simple cached method lookup.
   *
   *  Methods are first attempted to be located in a global cache
   *  but if it fails, the slower HierarchyResolver lookup is used.
   *  A located method is added to the cache for next time.
   *
   *  @see vm/global_cache.hpp
   *
   */
  class GlobalCacheResolver : public HierarchyResolver
  {
  public:   /* Ctors */

    virtual ~GlobalCacheResolver() { }

  public:   /* Interface */

    /**
     *  Cached method lookup path.
     *
     *  The method is first attempted to be located in the globally
     *  which the lookup *starts* as the key. If the method is found
     *  in the cache, the cached information is used. If not, the
     *  fallback is to look it up using HierarchyResolver. If the
     *  slower path locates the method, it is stored in the cache.
     *  Either way, the Executable object and the Module in which
     *  the Executable was originally located are stored in the
     *  Message.
     *
     *  @note     the cache is indexed using the name and Module where
     *            the lookup *started*. The information stored includes
     *            the Module in which the Executable was *found*.
     *
     *  @returns  true if the method was found, false otherwise.
     */
    virtual bool resolve(STATE, Message& msg);
  };


  /**
   *  Lookup path optimized to a specific SendSite.
   *
   *  A more specific optimization where the SendSite stores an
   *  instance of SpecializedResolver that directly stores the
   *  located method. If the Module given as the lookup starting
   *  point does not match, the fallback is GlobalCacheResolver.
   *
   *  @note       Not currently in use.
   */
  class SpecializedResolver : public GlobalCacheResolver
  {
  public:   /* Ctors */

    virtual ~SpecializedResolver() { }

  public:   /* Interface */

    /**
     *  Lookup specialized for a particular SendSite.
     *
     *  If the Module to look up from in the Message is the same
     *  as the one stored in the instance variable klass, the
     *  stored Executable and Module are used. If there is no
     *  stored method yet, or it does not match, GlobalCacheResolver
     *  is used to find the method. If it is found, the info
     *  is recorded here for the next time. The located Executable
     *  and the Module it was located in are stored in the Message.
     *
     *  @returns  true if method located, false otherwise.
     */
    virtual bool resolve(STATE, Message& msg);

  public:   /* Instance Variables */

    /** Module the Message should give to look up from */
    Module*     klass;
    /** Cached method object */
    Executable* method;
    /** Module in which the method was located */
    Module*     mod;
  };
}

#endif

