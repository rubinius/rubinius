#ifndef RBX_VIRTUAL_HPP
#define RBX_VIRTUAL_HPP

namespace rubinius {
  class ObjectCallback {
  public:
    STATE;

    ObjectCallback(STATE);
    virtual ~ObjectCallback();
    virtual void call(OBJECT) = 0;
  };
}

#endif
