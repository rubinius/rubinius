#ifndef RBX_VIRTUAL_HPP
#define RBX_VIRTUAL_HPP

namespace rubinius {

  class Object;

  class ObjectCallback {
  public:
    STATE;

    ObjectCallback(STATE);
    virtual ~ObjectCallback();
    virtual void call(Object*) = 0;
  };
}

#endif
